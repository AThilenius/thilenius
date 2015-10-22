// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>

#include "base/arguments.h"
#include "base/input.hh"
#include "base/log.h"
#include "base/path.h"
#include "base/value_of.hh"
#include "cloud/fiber/fiber_client.h"

DEFINE_string(fiber_ip, "localhost", "The ip enpoint of the Fiber server");
DEFINE_int32(fiber_port, 2600, "The port for the Fiber server");
DEFINE_string(fiber_route, "/", "The route of the Fiber server");
DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");

using ::thilenius::base::Arguments;
using ::thilenius::base::Input;
using ::thilenius::base::Path;
using ::thilenius::base::ValueOf;
using ::thilenius::cloud::fiber::FiberClient;

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc < 2) {
    LOG(FATAL) << "Usage: fiber <command> <command_args>";
  }
  std::string root_path = Path::CurrentPath();
  std::string command = argv[1];
  std::vector<std::string> args = Arguments::ToVector(argc - 2, &argv[2]);
  FiberClient fiber_client;
  fiber_client.Connect(FLAGS_fiber_ip, FLAGS_fiber_port, FLAGS_fiber_route,
                       FLAGS_sentinel_ip, FLAGS_sentinel_port,
                       FLAGS_sentinel_route).GetOrDie();
  if (command == "create") {
    // ==  CRETE  ==============================================================
    ::fiber::proto::Cord cord =
        fiber_client.CreateCord(root_path, args[0]).GetOrDie();
    LOG(INFO) << cord.uuid;
  } else if (command == "write") {
    // ==  WRITE  ==============================================================
    ::fiber::proto::Cord cord = fiber_client.GetCord(args[0]).GetOrDie();
    ::fiber::proto::Grain grain;
    grain.channel = 1;
    grain.data = args[1];
    fiber_client.WriteCord(cord, {grain});
  } else if (command == "read") {
    // ==  READ  ===============================================================
    ::fiber::proto::Cord cord = fiber_client.GetCord(args[0]).GetOrDie();
    for (int i = 0; true;) {
      auto results = fiber_client.ReadCordGTEIndex(cord, i).GetOrDie();
      i += results.size();
      for (const auto& result : results) {
        LOG(INFO) << result;
      }
    }
  } else if (command == "close") {
    // ==  CLOSE  ==============================================================
    ::fiber::proto::Cord cord = fiber_client.GetCord(args[0]).GetOrDie();
    fiber_client.CloseCord(cord).GetOrDie();
  } else {
    LOG(FATAL) << "Unknown CLI command " << command;
  }
}
