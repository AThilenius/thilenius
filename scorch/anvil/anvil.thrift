//==============================================================================
//==  ANVIL PROJECT  ===========================================================
//==============================================================================
include "scorch/cloud/crucible/crucible.thrift"

namespace cpp anvil

struct Test {
  1: bool did_pass;
  2: string name;
  3: string assert_message;
}

struct TestCase {
  1: bool did_pass;
  2: string name;
  3: i32 points_earned;
  4: i32 points_possible;
  5: i32 points_denominator;
  6: list<Test> tests;
}

struct TestRunReport {
  1: i32 total_points_earned;
  2: i32 total_points_possible;
  3: i32 total_points_denominator;
  4: list<TestCase> test_cases;
}

struct ProjectState {
  1: string project_uuid;
  2: string user_token;
  3: string last_commit_hash;
}

struct AnvilRunReport {
  1: ProjectState project_state;
  2: TestRunReport test_run_report;
}
