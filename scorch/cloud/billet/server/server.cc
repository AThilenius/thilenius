// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>

#include "base/log.h"
#include "cloud/utils/thrift_http_server.hh"
#include "scorch/cloud/billet/Billet.h"
#include "scorch/cloud/billet/billet_types.h"
#include "scorch/cloud/billet/server/billet_handler.h"

DEFINE_int32(server_port, 2300, "The port for the Billet server.");

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::scorch::cloud::billet::server::BilletHandler;

int main(int argc, char **argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  ThriftHttpServer<BilletHandler, ::billet::proto::BilletProcessor> http_server(
      FLAGS_server_port, "/");
  LOG(INFO) << "Starting server on port " << FLAGS_server_port;
  http_server.ServeBlocking();
}
