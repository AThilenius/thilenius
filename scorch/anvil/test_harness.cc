// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/test_harness.h"

#include <memory>
#include <sstream>

#include "base/json.h"
#include "base/process.h"
#include "base/regex.h"

using ::thilenius::base::Process;
using ::thilenius::base::Regex;

namespace thilenius {
namespace scorch {
namespace anvil {

TestHarness::TestHarness(const std::string& runnable_path, bool silent)
    : runnable_path_(runnable_path), silent_(silent) {}

ValueOf<::anvil::TestRunReport> TestHarness::RunAllTests() {
  try {
    std::stringstream cout_stream;
    std::stringstream cerr_stream;
    Process::ExecuteCommandSync(
        runnable_path_, {"--eport_json"},
        [&cout_stream](std::string line) { cout_stream << line; },
        [&cerr_stream](std::string line) { cerr_stream << line; });
    if (!silent_) {
      std::cout << cout_stream;
    }
    // Find JSON by regex
    std::string regex_string = "<JSON>(.*?)</JSON>";
    ValueOf<std::string> json_string =
        Regex::FindFirstMatch(cerr_stream.str(), regex_string);
    if (!json_string.IsValid()) {
      return {::anvil::TestRunReport(), "Failed to parse JSON from stderr"};
    }
    ::nlohmann::json json = ::nlohmann::json::parse(json_string.GetOrDie());
    // Build out Thrift objects from it
    ::anvil::TestRunReport test_run_report;
    test_run_report.total_points_earned =
        json["total_points_earned"].get<int>();
    test_run_report.total_points_possible =
        json["total_points_possible"].get<int>();
    test_run_report.total_points_denominator =
        json["total_points_denominator"].get<int>();
    for (const auto& test_case_json : json["test_cases"]) {
      ::anvil::TestCase test_case;
      test_case.did_pass = test_case_json["did_pass"].get<bool>();
      test_case.name = test_case_json["name"].get<std::string>();
      test_case.points_earned = test_case_json["points_earned"].get<int>();
      test_case.points_possible = test_case_json["points_possible"].get<int>();
      test_case.points_denominator =
          test_case_json["points_denominator"].get<int>();
        for (const auto& test_json : test_case_json["tests"]) {
          ::anvil::Test test;
          test.did_pass = test_json["did_pass"].get<bool>();
          test.name = test_json["name"].get<std::string>();
          test.assert_message = test_json["assert_message"].get<std::string>();
          test_case.tests.emplace_back(std::move(test));
        }
      test_run_report.test_cases.emplace_back(std::move(test_case));
    }
    return {std::move(test_run_report)};
  } catch (...) {
    return {::anvil::TestRunReport(), "Failed to execute " + runnable_path_};
  }
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
