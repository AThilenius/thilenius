// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_H_
#define SCORCH_ANVIL_ANVIL_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "base/macros.h"

namespace thilenius {
namespace scorch {
namespace anvil {

class Anvil {
 public:
  Anvil(int* argc, char*** argv);
  ~Anvil() = default;

  // Sets the points for a specific test case
  Anvil& SetPoints(const std::string& test_name, int denominator,
                   int points_possible);

  // Adds a source file that can be modified by the user
  Anvil& AddUserSource(const std::string& relative_path,
                       const std::string& md5);

  // Adds a source file that cannot be modified by the user
  Anvil& AddFrozenSource(const std::string& relative_path,
                         const std::string& md5);

  // Execute the configured Anvil stack
  int Execute();

 private:
  DISALLOW_COPY_AND_ASSIGN(Anvil);

  int* argc_;
  char*** argv_;
  std::unordered_map<std::string, std::tuple<int, int>> points_;
  std::vector<std::tuple<std::string, std::string>> user_source_files_;
  std::vector<std::tuple<std::string, std::string>> frozen_source_files_;
};  // class Anvil

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_H_
