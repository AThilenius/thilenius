// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/test_harness.h"

#include <memory>
#include <sstream>

#include "base/gtest/gtest.h"
#include "base/log.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/transport/TBufferTransports.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;
using ::thilenius::base::ConsoleColor;
using ::apache::thrift::transport::TMemoryBuffer;
using ::apache::thrift::protocol::TJSONProtocol;

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
    points_possible_total_ += std::get<1>(points.second);
  }
}

void TestHarness::RunAllTests() {
  // Remove standard harness
  UnitTest& unit_test = *UnitTest::GetInstance();
  TestEventListeners& listeners = unit_test.listeners();
  delete listeners.Release(listeners.default_result_printer());

  int ret_val = RUN_ALL_TESTS();
  PrintToStderr();
  ::anvil::snapshot::RunReport run_report = GetRunReport();
  boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
  TJSONProtocol json_protocol(buffer);
  run_report.write(&json_protocol);
  // TODO(athilenius): Use this, send-er out to "The server"!
  //LOG(INFO) << "Got JSON string:";
  //LOG(INFO) << buffer->getBufferAsString();
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

::anvil::snapshot::RunReport TestHarness::GetRunReport() {
  UnitTest& unit_test = *UnitTest::GetInstance();
  ::anvil::snapshot::RunReport run_report;
  int points_earned = 0;
  for (int i = 0; i < unit_test.total_test_case_count(); i++) {
    const TestCase& test_case = *unit_test.GetTestCase(i);
    const auto& iter = points_map_.find(test_case.name());
    int denominator = 0;
    int points_possible = 0;
    if (iter != points_map_.end()) {
      denominator = std::get<0>(iter->second);
      points_possible = std::get<1>(iter->second);
    }
    ::anvil::snapshot::TestCase snapshot_test_case;
    snapshot_test_case.did_pass = test_case.Passed();
    snapshot_test_case.name = test_case.name();
    snapshot_test_case.points_possible = points_possible;
    snapshot_test_case.points_denominator = denominator;
    if (test_case.Passed()) {
      points_earned += points_possible;
      snapshot_test_case.points_earned = points_possible;
    } else {
      snapshot_test_case.points_earned = 0;
    }
    for (int j = 0; j < test_case.total_test_count(); j++) {
      const TestInfo& test_info = *test_case.GetTestInfo(j);
      ::anvil::snapshot::Test snapshot_test;
      snapshot_test.did_pass = test_info.result()->Passed();
      for (int k = 0; k < test_info.result()->total_part_count(); k++) {
        snapshot_test.message_parts.push_back(
            test_info.result()->GetTestPartResult(k).message());
      }
      snapshot_test_case.tests.push_back(snapshot_test);
    }
    run_report.test_cases.push_back(snapshot_test_case);
  }
  run_report.total_points_earned = points_earned;
  run_report.total_points_possible = points_possible_total_;
  run_report.total_points_denominator = points_denominator_total_;
  return std::move(run_report);
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
