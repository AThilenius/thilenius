// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>

#include "base/arguments.h"
#include "base/input.hh"
#include "base/log.h"
#include "base/path.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_client.h"

DEFINE_string(sentinel_ip, "localhost",
              "The ip enpoint of the Sentinel server");
DEFINE_int32(sentinel_port, 2100, "The port for the Sentinel server");
DEFINE_string(sentinel_route, "/", "The route of the Sentinel server");

using ::thilenius::base::Arguments;
using ::thilenius::base::Input;
using ::thilenius::base::Path;
using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelClient;

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace cli {

int Author(const std::string root_path, const std::vector<std::string>& args) {
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route).GetOrDie();
  ::sentinel::proto::Token token =
      sentinel_client.LoginUserFromCin().GetOrDie();
  LOG(INFO) << "Login successful";
  ::sentinel::proto::Token authored_token =
      sentinel_client.AuthorToken(token, 0).GetOrDie();
  LOG(INFO) << "Secondary token authored and authenticated with Sentinel: "
            << token;
  if (!sentinel_client.SaveProjectToken(authored_token, root_path)) {
    LOG(ERROR) << "Failed to create keyfile";
  }
  return 0;
}

int Login(const std::string root_path, const std::vector<std::string>& args) {
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route).GetOrDie();
  ::sentinel::proto::Token token =
      sentinel_client.LoginUserFromCin().GetOrDie();
  LOG(INFO) << "Login successful";
  ::sentinel::proto::User user_partial;
  user_partial.uuid = token.user_uuid;
  ::sentinel::proto::User user =
      sentinel_client.FindUser(token, user_partial).GetOrDie();
  LOG(INFO) << "First name: " << user.first_name;
  LOG(INFO) << "Last name: " << user.last_name;
  LOG(INFO) << "Email Address: " << user.email_address;
  LOG(INFO) << "Permission Level: " << user.permission_level;
  return 0;
}

int Create(const std::string root_path, const std::vector<std::string>& args) {
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route).GetOrDie();
  ::sentinel::proto::User user;
  LOG(INPUT) << "First name: ";
  user.first_name = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Last name: ";
  user.last_name = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Email address: ";
  user.email_address = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Password: ";
  std::string password = Input::WaitOnceOrDie<std::string>();
  ::sentinel::proto::Token ptoken =
      sentinel_client.CreateUser(user, password).GetOrDie();
  LOG(INFO) << "Account created!";
  return 0;
}

}  // namespace cli
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc < 2) {
    LOG(FATAL) << "Usage: sentinel <command> <command_args>";
  }
  std::string root_path = Path::CurrentPath();
  std::string command = argv[1];
  std::vector<std::string> args = Arguments::ToVector(argc - 2, &argv[2]);
  if (command == "create") {
    ::thilenius::cloud::sentinel::cli::Create(root_path, args);
  } else if (command == "login") {
    ::thilenius::cloud::sentinel::cli::Login(root_path, args);
  } else if (command == "author") {
    ::thilenius::cloud::sentinel::cli::Author(root_path, args);
  } else {
    LOG(FATAL) << "Unknown CLI command " << command;
  }
}
