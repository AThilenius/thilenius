// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>

#include "base/log.h"
#include "cloud/fiber/Fiber.h"
#include "cloud/fiber/server/fiber_handler.h"
#include "cloud/utils/thrift_http_server.hh"

DEFINE_int32(server_port, 2600, "The port for the Fiber server.");

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::cloud::fiber::server::FiberHandler;

int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ThriftHttpServer<FiberHandler, ::fiber::proto::FiberProcessor> http_server(
      FLAGS_server_port, "/");
  LOG(INFO) << "Starting Fiber Server on port " << FLAGS_server_port;
  http_server.ServeBlocking();
}
