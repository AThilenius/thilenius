// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/test_harness.h"

#include <sstream>

#include "base/gtest/gtest.h"
#include "base/log.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

using thilenius::base::ConsoleColor;

namespace thilenius {
namespace scorch {
namespace anvil {

TestHarness::TestHarness(
    const std::unordered_map<std::string, std::tuple<int, int>> points_map)
    : points_map_(points_map),
      points_denominator_total_(0),
      points_possible_total_(0) {
  for (const auto& points : points_map) {
    points_denominator_total_ += std::get<0>(points.second);
    points_possible_total_ += std::get<0>(points.second);
  }
}

void TestHarness::RunAllTests() {
  // Remove standard harness
  UnitTest& unit_test = *UnitTest::GetInstance();
  TestEventListeners& listeners = unit_test.listeners();
  delete listeners.Release(listeners.default_result_printer());

  int ret_val = RUN_ALL_TESTS();
  PrintToStderr();
}

void TestHarness::PrintToStderr() {
  UnitTest& unit_test = *UnitTest::GetInstance();
  int points_earned = 0;
  // Use manual IO
  std::cout << ConsoleColor::BLUE << "- Run Report" << std::endl;
  std::cout << "|" << ConsoleColor::WHITE << std::endl;
  for (int i = 0; i < unit_test.total_test_case_count(); i++) {
    const TestCase& test_case = *unit_test.GetTestCase(i);
    const auto& iter = points_map_.find(test_case.name());
    int denominator = 0;
    int points_possible = 0;
    if (iter != points_map_.end()) {
      denominator = std::get<0>(iter->second);
      points_possible = std::get<1>(iter->second);
    }
    std::cout << ConsoleColor::BLUE << "|";
    if (test_case.Passed()) {
      std::cout << ConsoleColor::GREEN;
      std::cout << "   - [" << points_possible << "/" << denominator << "] "
                << test_case.name() << std::endl;
      points_earned += points_possible;
    } else {
      std::cout << ConsoleColor::RED;
      std::cout << "   - [0/" << denominator << "] " << test_case.name()
                << std::endl;
    }
    for (int j = 0; j < test_case.total_test_count(); j++) {
      const TestInfo& test_info = *test_case.GetTestInfo(j);
      std::cout << ConsoleColor::BLUE << "|";
      std::cout << (test_case.Passed() ? ConsoleColor::GREEN
                                       : ConsoleColor::RED) << "   |";
      if (test_info.result()->Passed()) {
        std::cout << ConsoleColor::GREEN << "   - " << test_info.name()
                  << std::endl;
      } else {
        std::cout << ConsoleColor::RED << "   - " << test_info.name()
                  << std::endl;
        for (int k = 0; k < test_info.result()->total_part_count(); k++) {
          std::istringstream iss(
              test_info.result()->GetTestPartResult(k).message());
          std::string line;
          while (std::getline(iss, line)) {
            std::cout << ConsoleColor::BLUE << "|" << ConsoleColor::RED
                      << "   |   |" << ConsoleColor::YELLOW << "   " << line
                      << std::endl;
          }
        }
      }
    }
    std::cout << ConsoleColor::BLUE << "|";
    std::cout << (test_case.Passed() ? ConsoleColor::GREEN : ConsoleColor::RED)
              << "   - (" << points_possible << " points possible)"
              << std::endl;
    std::cout << ConsoleColor::BLUE << "|" << std::endl;
  }
  std::cout << ConsoleColor::BLUE << "- [" << points_earned << "/"
            << points_denominator_total_ << "]  (" << points_possible_total_
            << " points possible)" << ConsoleColor::WHITE << std::endl;
  std::cout.flush();
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
