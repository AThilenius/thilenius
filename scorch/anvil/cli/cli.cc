// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>
#include <string>
#include <vector>

#include "base/arguments.h"
#include "base/log.h"
#include "base/path.h"
#include "scorch/anvil/anvil_client.h"

DEFINE_string(blaze_ip, "192.168.59.3", "The ip enpoint of the blaze server");
DEFINE_int32(blaze_port, 2500, "The port for the blaze server");

using ::thilenius::base::Arguments;
using ::thilenius::base::Path;
using ::thilenius::scorch::anvil::AnvilClient;

namespace thilenius {
namespace scorch {
namespace anvil {
namespace cli {

int Process(const std::string root_path, const std::vector<std::string>& args) {
  AnvilClient anvil_client;
  anvil_client.Connect(FLAGS_blaze_ip, FLAGS_blaze_port).GetOrDie();
  ::blaze::proto::BlazeRequestWrapper blaze_request;
  blaze_request.user_uuid = "UserUUID!";
  blaze_request.minecraft_username = "alth3531";
  blaze_request.command_name = "action_target_invoke";
  blaze_request.args_json.emplace_back(args[0]);
  blaze_request.args_json.emplace_back(args[1]);
  ::blaze::proto::BlazeResponseWrapper response =
      anvil_client.Process(blaze_request).GetOrDie();
  LOG(INFO) << "Got back JSON: " << response.json;
  return 0;
}

}  // namespace cli
}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc < 2) {
    LOG(FATAL) << "Usage: blaze <command> <command_args>";
  }
  std::string root_path = Path::CurrentPath();
  std::string command = argv[1];
  std::vector<std::string> args = Arguments::ToVector(argc - 2, &argv[2]);
  if (command == "process") {
    return ::thilenius::scorch::anvil::cli::Process(root_path, args);
  } else {
    LOG(FATAL) << "Unknown CLI command " << command;
  }
}
