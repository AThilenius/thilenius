// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_PROCESS_H_
#define BASE_PROCESS_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

#include "base/blocking_stream.hh"
#include "base/value_of.hh"

using ::thilenius::base::BlockingStream;
using ::thilenius::base::ValueOf;

namespace thilenius {
namespace base {

class Process;
typedef std::shared_ptr<Process> ProcessPtr;

class Process {
 public:
  struct OStreamToken {
    bool is_err_stream;
    std::string content;
  };

  Process();

  static std::string FindExecutable(const std::string& name);

  static int ExecuteCommandSync(const std::string& command,
                                const std::vector<std::string> args,
                                bool silent);

  static int ExecuteCommandSync(
      const std::string& command, const std::vector<std::string> args,
      const std::function<void(std::string)> stdout_handler);

  static int ExecuteCommandSync(
      const std::string& command, const std::vector<std::string> args,
      const std::function<void(std::string)> stdout_handler,
      const std::function<void(std::string)> stderr_handler);

  static ProcessPtr FromFork(std::function<int()> child_task);

  static ProcessPtr FromExecv(const std::string& full_path,
                           const std::vector<std::string> args);

  bool Execute(bool blocking, int timeout_ms);

  // Don't use with Execute(4)
  bool WriteCin(const std::string& value);

  ValueOf<std::vector<OStreamToken>> ReadOutputAfterIndex(int index);

  int GetExitCode();

 private:
  int Wait(int timeout_ms);

  ValueOf<OStreamToken> ReadLineFrom(std::string* read_ahead_buffer,
                                     int fd) const;

  std::function<int()> child_task_;
  pid_t pid_;
  int pipes_[3][2];
  int exit_code;
  std::chrono::time_point<std::chrono::system_clock> start_time_;
  BlockingStream<OStreamToken> blocking_ostream_;
  std::string cout_read_ahead_buffer_;
  std::string cerr_read_ahead_buffer_;
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_PROCESS_H_
