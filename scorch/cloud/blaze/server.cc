// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>

#include "base/log.h"
#include "cloud/utils/thrift_http_server.hh"
#include "scorch/cloud/blaze/Blaze.h"
#include "scorch/cloud/blaze/blaze_handler.h"
#include "scorch/cloud/blaze/blaze_types.h"

DEFINE_int32(server_port, 2400, "The port for the Blaze server.");

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::scorch::cloud::blaze::BlazeHandler;

int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ThriftHttpServer<BlazeHandler, ::blaze::proto::BlazeProcessor> http_server(
      FLAGS_server_port, "/");
  LOG(INFO) << "Blaze starting server on port " << FLAGS_server_port;
  http_server.ServeBlocking();
}
