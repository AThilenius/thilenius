// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_PROCESS_H_
#define BASE_PROCESS_H_

#include <functional>
#include <string>
#include <vector>

namespace thilenius {
namespace base {

class Process {
 public:
  static std::string FindExecutable(const std::string& name);
  static int ExecuteCommandSync(const std::string& command,
                                const std::vector<std::string> args,
                                bool silent);
  static int ExecuteCommandSync(
      const std::string& command, const std::vector<std::string> args,
      const std::function<void(std::string)> stdout_handler);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_PROCESS_H_
