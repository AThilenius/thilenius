// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/arguments.h"
#include "base/gflags/gflags.h"
#include "base/log.h"
#include "base/path.h"
#include "cloud/sentinel/cli/author.h"
#include "cloud/sentinel/cli/create.h"
#include "cloud/sentinel/cli/login.h"
#include "cloud/sentinel/sentinel_user.h"

using ::thilenius::base::Arguments;
using ::thilenius::base::Path;

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::thilenius::cloud::sentinel::SentinelUser::CheckConnectionOrDie();
  if (argc < 2) {
    LOG(FATAL) << "Usage: sentinel <command> <command_args>";
  }
  std::string root_path = Path::CurrentPath();
  std::string command = argv[1];
  std::vector<std::string> args = Arguments::ToVector(argc - 2, &argv[2]);
  if (command == "create") {
    ::thilenius::cloud::sentinel::cli::Create(root_path, args);
  } else if (command == "login"){
    ::thilenius::cloud::sentinel::cli::Login(root_path, args);
  } else if (command == "author"){
    ::thilenius::cloud::sentinel::cli::Author(root_path, args);
  } else {
    LOG(FATAL) << "Unknown CLI command " << command;
  }
}
