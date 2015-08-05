// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_H_
#define SCORCH_ANVIL_ANVIL_H_

#include <string>
#include <unordered_map>

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

  // Execute the configured Anvil stack
  int Execute();

 private:
  DISALLOW_COPY_AND_ASSIGN(Anvil);

  int* argc_;
  char*** argv_;
  std::unordered_map<std::string, std::tuple<int, int>> points_;
};  // class Anvil

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_H_
