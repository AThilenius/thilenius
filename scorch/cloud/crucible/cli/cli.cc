// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <vector>
#include <string>

#include "base/arguments.h"
#include "base/gflags/gflags.h"
#include "base/log.h"
#include "base/path.h"
#include "scorch/cloud/crucible/cli/create.h"
#include "scorch/cloud/crucible/cli/status.h"

using ::thilenius::base::Arguments;
using ::thilenius::base::Path;

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (argc < 2) {
    LOG(FATAL) << "Usage: crucible <command> <command_args>";
  }
  std::string root_path = Path::CurrentPath();
  std::string command = argv[1];
  std::vector<std::string> args = Arguments::ToVector(argc - 2, &argv[2]);
  if (command == "create") {
    return ::thilenius::scorch::cloud::crucible::cli::Create(root_path, args);
  } else if (command == "status") {
    return ::thilenius::scorch::cloud::crucible::cli::Status(root_path, args);
  } else {
    LOG(FATAL) << "Unknown CLI command " << command;
  }
}
