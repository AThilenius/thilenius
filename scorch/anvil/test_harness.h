// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_TEST_HARNESS_H_
#define SCORCH_ANVIL_TEST_HARNESS_H_

#include <string>
#include <tuple>
#include <unordered_map>

#include "base/macros.h"

namespace thilenius {
namespace scorch {
namespace anvil {

class TestHarness {
 public:
  explicit TestHarness(
      const std::unordered_map<std::string, std::tuple<int, int>> points_map);
  void RunAllTests();

 private:
  DISALLOW_COPY_AND_ASSIGN(TestHarness);

  void PrintToStderr();

  const std::unordered_map<std::string, std::tuple<int, int>> points_map_;
  int points_denominator_total_;
  int points_possible_total_;
};  // class TestHarness

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_TEST_HARNESS_H_
