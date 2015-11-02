// Copyright 2015 Alec Thilenius
// All rights reserved.

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

#define READ 0
#define WRITE 1

namespace thilenius {
namespace base {

Process::Process() : exit_code(-1) {}

std::string Process::FindExecutable(const std::string& name) {
  if (File::Exists(name)) {
    return name;
  }
  std::string full_path;
  try {
    full_path = ::boost::process::find_executable_in_path(name);
  } catch (::boost::filesystem::filesystem_error& ex) {
  }
  return full_path;
}

int Process::ExecuteCommandSync(const std::string& command,
                                const std::vector<std::string> args,
                                bool silent) {
  std::function<void(std::string)> line_handler =
      silent ? [](std::string line) {}
             : [](std::string line) { std::cout << line << std::endl; };
  return ExecuteCommandSync(command, args, line_handler);
}

int Process::ExecuteCommandSync(
    const std::string& command, const std::vector<std::string> args,
    const std::function<void(std::string)> stdout_handler) {
  std::function<void(std::string)> stderr_handler =
      [&stdout_handler](std::string line) { stdout_handler(line); };
  return ExecuteCommandSync(command, args, stdout_handler, stderr_handler);
}

int Process::ExecuteCommandSync(
    const std::string& command, const std::vector<std::string> args,
    const std::function<void(std::string)> stdout_handler,
    const std::function<void(std::string)> stderr_handler) {
  std::string exec = FindExecutable(command);
  if (String::Blank(exec)) {
    return 1;
  }
  // Add command path to start of args
  std::vector<std::string> args_with_path = {exec};
  args_with_path.insert(args_with_path.end(), args.begin(), args.end());
  ::boost::process::context ctx;
  ctx.environment = ::boost::process::self::get_environment();
  ctx.stdout_behavior = ::boost::process::capture_stream();
  ctx.stderr_behavior = ::boost::process::capture_stream();
  ::boost::process::child child =
      ::boost::process::launch(exec, args_with_path, ctx);
  ::boost::process::pistream& stdout_stream = child.get_stdout();
  ::boost::process::pistream& stderr_stream = child.get_stderr();
  std::string stdout_line;
  std::string stderr_line;
  while (std::getline(stdout_stream, stdout_line) ||
         std::getline(stderr_stream, stderr_line)) {
    if (!String::Blank(stdout_line)) {
      stdout_handler(stdout_line);
      stdout_line = "";
    }
    if (!String::Blank(stderr_line)) {
      stderr_handler(stderr_line);
      stderr_line = "";
    }
  }
  ::boost::process::status status = child.wait();
  return (status.exited() ? !!status.exit_status() : EXIT_FAILURE);
}

ProcessPtr Process::FromFork(std::function<int()> child_task) {
  ProcessPtr process(new Process());
  process->child_task_ = child_task;
  process->pid_ = 0;
  return process;
}

ProcessPtr Process::FromExecv(const std::string& full_path,
                              const std::vector<std::string> args) {
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
  return Process::FromFork(child_task);
}

bool Process::Execute(bool blocking, int timeout_ms) {
  // pipes for parent to write and read
  pipe(pipes_[CIN]);
  pipe(pipes_[COUT]);
  pipe(pipes_[CERR]);
  pid_ = fork();
  if (pid_ < 0) {
    return false;
  }
  if (pid_ == 0) {
    dup2(pipes_[CIN][READ], STDIN_FILENO);
    dup2(pipes_[COUT][WRITE], STDOUT_FILENO);
    dup2(pipes_[CERR][WRITE], STDERR_FILENO);
    exit(child_task_());
  } else {
    close(pipes_[CIN][READ]);
    close(pipes_[COUT][WRITE]);
    close(pipes_[CERR][WRITE]);
    start_time_ = std::chrono::system_clock::now();
    // Wait if blocking, otherwise source end from wait
    if (blocking) {
      // If we are blocking mode, then source cout/cerr directly
      exit_code = Wait(timeout_ms);
      while (true) {
        auto cout_line_value =
            ReadLineFrom(&cout_read_ahead_buffer_, pipes_[COUT][READ]);
        if (!cout_line_value.IsValid()) {
          break;
        }
        blocking_ostream_.Write(cout_line_value.GetOrDie());
      }
      while (true) {
        auto cerr_line_value =
            ReadLineFrom(&cerr_read_ahead_buffer_, pipes_[CERR][READ]);
        if (!cerr_line_value.IsValid()) {
          break;
        }
        blocking_ostream_.Write(cerr_line_value.GetOrDie());
      }
      return exit_code == 0;
    } else {
      // Source from cout
      blocking_ostream_.SourceFrom([this]() -> ValueOf<OStreamToken> {
        if (this) {
          return ReadLineFrom(&cout_read_ahead_buffer_, pipes_[COUT][READ]);
        } else {
          return {OStreamToken(), "Process was destroyed"};
        }
      });
      // Source from cerr
      blocking_ostream_.SourceFrom([this]() -> ValueOf<OStreamToken> {
        if (this) {
          return ReadLineFrom(&cout_read_ahead_buffer_, pipes_[COUT][READ]);
        } else {
          return {OStreamToken(), "Process was destroyed"};
        }
      });
      blocking_ostream_.EndFrom(
          [this, timeout_ms]() { exit_code = Wait(timeout_ms); });
    }
    return true;
  }
}

void Process::Kill(bool blocking) {
  kill(pid_, SIGKILL);
  if (blocking) {
    waitpid(pid_, 0, 0);
  }
}

bool Process::WriteCin(const std::string& value) {
  return write(pipes_[CIN][WRITE], value.c_str(), value.length()) ==
         value.length();
}

ValueOf<std::vector<Process::OStreamToken>> Process::ReadOutputAfterIndex(
    int index) {
  return blocking_ostream_.ReadItemsAfterIndex(index);
}

int Process::GetExitCode() { return exit_code; }

int Process::Wait(int timeout_ms) {
  // This is a simple spin-wait because i'm too lazy to do anything better
  while (true) {
    int status;
    int rc = waitpid(pid_, &status, WNOHANG);
    if (rc == -1) {
      // Assume exit failure. I guess...
      return -2;
    }
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    // Terminate time (timeout)
    if (timeout_ms > 0 &&
        (start_time_ + std::chrono::milliseconds(timeout_ms)) <=
            std::chrono::system_clock::now()) {
      kill(pid_, SIGKILL);
      waitpid(pid_, 0, 0);
      return -1;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

ValueOf<Process::OStreamToken> Process::ReadLineFrom(
    std::string* read_ahead_buffer, int fd) const {
  std::string::iterator pos;
  while ((pos = find(read_ahead_buffer->begin(), read_ahead_buffer->end(),
                     '\n')) == read_ahead_buffer->end()) {
    char buf[1025];
    int n = read(fd, buf, 1024);
    if (n <= 0) {
      // Return anything left in the read ahead buffer
      if (read_ahead_buffer->length() > 0) {
        OStreamToken ostream_token;
        ostream_token.is_err_stream = (fd == pipes_[CERR][READ]);
        ostream_token.content = *read_ahead_buffer;
        read_ahead_buffer->clear();
        return std::move(ostream_token);
      } else {
        return {OStreamToken(), "End of cout stream"};
      }
    }
    buf[n] = 0;
    *read_ahead_buffer += buf;
  }
  // Split the buffer around '\n' found and return first part.
  OStreamToken ostream_token;
  ostream_token.is_err_stream = (fd == pipes_[CERR][READ]);
  ostream_token.content = std::string(read_ahead_buffer->begin(), pos) + "\n";
  *read_ahead_buffer = std::string(pos + 1, read_ahead_buffer->end());
  return std::move(ostream_token);
}

}  // namespace base
}  // namespace thilenius
