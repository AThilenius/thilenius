//==============================================================================
//==  ANVIL PROJECT  ===========================================================
//==============================================================================
namespace cpp anvil.snapshot

// TODO(athilenius): Move this over to SourceControl service defs
struct AnvilSourceFile {
  1: string realative_path;
  2: string md5;
}

struct Test {
  1: bool did_pass;
  2: string name;
  3: list<string> message_parts;
}

struct TestCase {
  1: bool did_pass;
  2: string name;
  3: i32 points_earned;
  4: i32 points_possible;
  5: i32 points_denominator;
  6: list<Test> tests;
}

struct RunReport {
  1: i32 total_points_earned;
  2: i32 total_points_possible;
  3: i32 total_points_denominator;
  4: list<TestCase> test_cases;
}

struct AnvilSnapshot {
  1: string project_uuid;
  2: string user_token;
  3: string previous_md5;
  4: list<AnvilSourceFile> snapshot_files;
  5: list<AnvilSourceFile> frozen_files;
  6: RunReport run_report;
}
