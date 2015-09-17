// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "cloud/sentinel/Sentinel.h"
#include "cloud/sentinel/server/sentinel_handler.h"
#include "cloud/utils/thrift_http_server.hh"

DEFINE_int32(server_port, 80, "The port for the Sentinel server.");

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::cloud::sentinel::server::SentinelHandler;

int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ThriftHttpServer<SentinelHandler, ::sentinel::SentinelProcessor> http_server(
      FLAGS_server_port, "/");
  LOG(INFO) << "Starting server on port " << FLAGS_server_port;
  http_server.ServeBlocking();
}
