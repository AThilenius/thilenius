// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>

#include "base/log.h"
#include "cloud/utils/thrift_http_server.hh"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "scorch/cloud/crucible/server/crucible_handler.h"

DEFINE_int32(server_port, 2200, "The port for the Crucible server.");

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::scorch::cloud::crucible::server::CrucibleHandler;

int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ThriftHttpServer<CrucibleHandler, ::crucible::proto::CrucibleProcessor> http_server(
      FLAGS_server_port, "/");
  LOG(INFO) << "Starting server on port " << FLAGS_server_port;
  http_server.ServeBlocking();
}
