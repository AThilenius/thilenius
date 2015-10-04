// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_PROCESS_H_
#define BASE_PROCESS_H_

#include <chrono>
#include <functional>
#include <string>
#include <unistd.h>
#include <vector>

namespace thilenius {
namespace base {

class Process {
 public:
  Process() = default;

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

  static Process FromFork(std::function<int()> child_task);

  static Process FromExecv(const std::string& full_path,
                           const std::vector<std::string> args);

  bool Execute();

  int Wait(int timeout_ms);

  bool WriteCin(const std::string& value);

  std::string GetOut();

  std::string GetErr();

 private:
  std::function<int()> child_task_;
  pid_t pid_;
  int pipes_[3][2];
  std::chrono::time_point<std::chrono::system_clock> start_time_;
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_PROCESS_H_
