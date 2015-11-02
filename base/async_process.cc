// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/async_process.h"

#include <boost/process.hpp>
#include <cstdlib>
#include <iostream>
#include <limits.h>
#include <signal.h>
#include <sstream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "base/file.h"
#include "base/process.h"
#include "base/string.h"

#define CIN 0
#define COUT 1
#define CERR 2
#define CLOSE_STREAM 3

#define READ 0
#define WRITE 1

namespace thilenius {
namespace base {

AsyncProcess::AsyncProcess() : open_streams_(2), pid_(0) {}

AsyncProcessPtr AsyncProcess::FromFork(
    std::function<int()> child_task,
    std::function<void(int, std::string)> output_handler,
    std::function<void(int)> end_handler) {
  AsyncProcessPtr process(new AsyncProcess());
  if (!process->Execute(child_task, output_handler, end_handler)) {
    return AsyncProcessPtr(nullptr);
  }
  return process;
}

AsyncProcessPtr AsyncProcess::FromExecv(
    const std::string& full_path, const std::vector<std::string> args,
    std::function<void(int, std::string)> output_handler,
    std::function<void(int)> end_handler) {
  // Must be a vector of char* because execv is a crappy C call. I hate C
  std::function<int()> child_task = [full_path, args]() -> int {
    std::vector<char*> final_args(args.size() + 2);
    final_args[0] = const_cast<char*>(full_path.c_str());
    for (int i = 0; i != args.size(); ++i) {
      final_args[i + 1] = const_cast<char*>(&args[i][0]);
    }
    // Null terminated
    final_args[args.size() + 1] = 0;
    // Exec it
    return execv(full_path.c_str(), final_args.data());
  };
  return AsyncProcess::FromFork(child_task, output_handler, end_handler);
}

void AsyncProcess::Kill() {
  std::unique_lock<std::mutex> lock(mutex_);
  LOG(INFO) << "Int gave: " << kill(pid_, SIGINT);
  LOG(INFO) << "Stop gave: " << kill(pid_, SIGQUIT);
  LOG(INFO) << "KIll gave: " << kill(pid_, SIGKILL);
}

bool AsyncProcess::DidExit() {
  std::unique_lock<std::mutex> lock(mutex_);
  return open_streams_ == 0;
}

bool AsyncProcess::Execute(
    std::function<int()> child_task,
    std::function<void(int, std::string)> output_handler,
    std::function<void(int)> end_handler) {
  std::unique_lock<std::mutex> lock(mutex_);
  // pipes for parent to write and read
  pipe(pipes_[CIN]);
  pipe(pipes_[COUT]);
  pipe(pipes_[CERR]);
  pid_ = fork();
  if (pid_ < 0) {
    return false;
  }
  if (pid_ == 0) {
    // Child
    dup2(pipes_[CIN][READ], STDIN_FILENO);
    dup2(pipes_[COUT][WRITE], STDOUT_FILENO);
    dup2(pipes_[CERR][WRITE], STDERR_FILENO);
    exit(child_task());
  } else {
    // Parent
    close(pipes_[CIN][READ]);
    close(pipes_[COUT][WRITE]);
    close(pipes_[CERR][WRITE]);
    // Create a thread for cout
    std::thread([this]() {
      ReadLinesFrom(pipes_[COUT][READ], COUT);
    }).detach();
    // Create a thread for cerr
    std::thread([this]() {
      ReadLinesFrom(pipes_[CERR][READ], CERR);
    }).detach();
    // Create a thread to service cout, cerr and end handler from tokens queue
    std::thread([this, output_handler, end_handler]() {
      std::unique_lock<std::mutex> lock(mutex_);
      while (true) {
        while (!tokens_.size()) {
          cond_var_.wait(lock);
        }
        for (const auto& token : tokens_) {
          if (token.type == CLOSE_STREAM) {
            end_handler(token.exit_code);
            return;
          } else {
            output_handler(token.type, token.content);
          }
        }
        tokens_.clear();
      }
    }).detach();
    return true;
  }
}

void AsyncProcess::CloseStream() {
  std::unique_lock<std::mutex> lock(mutex_);
  open_streams_--;
  if (open_streams_ <= 0) {
    // We are the last stream to close, fetch the exit code and mark
    // process exited. Notify anyone waiting
    int status = -1;
    int exit_code = -1;
    int rc = waitpid(pid_, &status, 0);
    if (rc != -1) {
      exit_code = WEXITSTATUS(status);
    }
    tokens_.push_back({CLOSE_STREAM, exit_code, ""});
    cond_var_.notify_all();
  }
}

void AsyncProcess::ReadLinesFrom(int fd, int channel) {
  std::string read_ahead_buffer;
  while (true) {
    std::string::iterator pos;
    // While there is no newline characters
    while ((pos = find(read_ahead_buffer.begin(), read_ahead_buffer.end(),
                       '\n')) == read_ahead_buffer.end()) {
      char buf[1025];
      int n = read(fd, buf, 1024);
      if (n <= 0) {
        // Return anything left in the read ahead buffer
        if (read_ahead_buffer.length() > 0) {
          std::string content = read_ahead_buffer;
          read_ahead_buffer.clear();
          {
            std::unique_lock<std::mutex> lock(mutex_);
            tokens_.push_back({channel, -1, std::move(content)});
            cond_var_.notify_all();
          }
        } else {
          // read() returned negative AND the read ahead buffer has been
          // depleated Decrement one from the number of active streams
          CloseStream();
          return;
        }
      }
      buf[n] = 0;
      read_ahead_buffer += buf;
    }
    // Split the buffer around '\n' found and return first part.
    std::string content = std::string(read_ahead_buffer.begin(), pos) + "\n";
    read_ahead_buffer = std::string(pos + 1, read_ahead_buffer.end());
    {
      std::unique_lock<std::mutex> lock(mutex_);
      tokens_.push_back({channel, -1, std::move(content)});
      cond_var_.notify_all();
    }
  }
}

}  // namespace base
}  // namespace thilenius
