// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/anvil.h"

#include <iostream>
#include <memory>
#include <string>

#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "base/log.h"
#include "scorch/anvil/test_harness.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/transport/TFileTransport.h"
// DO NOT SUBMIT
#include <mongo/client/dbclient.h>

DEFINE_string(project_state_path, "project_state.json",
              "The filename of the anvil ProjectState");

using ::apache::thrift::protocol::TJSONProtocol;
using ::apache::thrift::transport::TMemoryBuffer;
using ::thilenius::base::File;

namespace thilenius {
namespace scorch {
namespace anvil {

Anvil::Anvil(int* argc, char*** argv) : argc_(argc), argv_(argv) {
  gflags::ParseCommandLineFlags(argc, argv, true);
  testing::InitGoogleTest(argc, *argv);
  testing::InitGoogleMock(argc, *argv);
}

void Anvil::AddFrozenSource(const std::string& relative_path) {
  frozen_files_.push_back(relative_path);
}

void Anvil::AddUserSource(const std::string& relative_path) {
  user_source_files_.push_back(relative_path);
}

void Anvil::SetPoints(const std::string& test_name, int denominator,
                      int points_possible) {
  points_[test_name] = std::tuple<int, int>(denominator, points_possible);
}

void SetProjectStatePath(const std::string& path) {}

void Anvil::Execute() {
  TestHarness harness(points_);
  harness.RunAllTests();

  // DO NOT SUBMIT
  //::mongo::client::initialize();
  //::mongo::DBClientConnection c;
  //c.connect("10.17.8.100");
}

::anvil::AnvilRunReport Anvil::GenerateAnvilRunReport() {
  ::anvil::AnvilRunReport anvil_run_report;
  anvil_run_report.project_state = LoadProjectState();
  anvil_run_report.source_files = LoadSourceFiles();
  anvil_run_report.frozen_files = LoadFrozenFiles();
  anvil_run_report.test_run_report = RunUnitTests();
  return std::move(anvil_run_report);
}

::anvil::ProjectState Anvil::LoadProjectState() {
  if (!File::Exists(FLAGS_project_state_path)) {
    LOG(FATAL) << "Failed to find ProjectState at " << FLAGS_project_state_path
               << ". See --project_state_path";
  }
  using ::apache::thrift::transport::TFileTransport;
  using ::apache::thrift::protocol::TJSONProtocol;
  boost::shared_ptr<TFileTransport> transport(
      new TFileTransport(FLAGS_project_state_path));
  boost::shared_ptr<TJSONProtocol> protocol(new TJSONProtocol(transport));
  // Load the ProtjectState from disk
  ::anvil::ProjectState project_state;
  project_state.read(protocol.get());
  return std::move(project_state);
}

::anvil::TestRunReport Anvil::RunUnitTests() {
  TestHarness test_harness(points_);
  test_harness.RunAllTests();
  return std::move(test_harness.GetTestRunReport());
}

std::vector<::crucible::FileInfo> Anvil::LoadFrozenFiles() {
  std::vector<::crucible::FileInfo> frozen_files;
  // MD5 all frozen files
  for (const auto& frozen_file : frozen_files_) {
    ::crucible::FileInfo file;
    file.realative_path = frozen_file;
    file.md5 = File::MD5OrDie(frozen_file);
    file.modify_timestamp = File::LastWriteTime(frozen_file);
    frozen_files.push_back(std::move(file));
  }
  return std::move(frozen_files);
}

std::vector<::crucible::SourceFile> Anvil::LoadSourceFiles() {
  std::vector<::crucible::SourceFile> source_files;
  // Load and MD5 all user files
  for (const auto& user_file : user_source_files_) {
    ::crucible::SourceFile source_file;
    source_file.file_info.realative_path = user_file;
    source_file.file_info.md5 = File::MD5OrDie(user_file);
    source_file.file_info.modify_timestamp = File::LastWriteTime(user_file);
    source_file.contents = File::ReadContentsOrDie(user_file);
    source_files.push_back(std::move(source_file));
  }
  return std::move(source_files);
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
