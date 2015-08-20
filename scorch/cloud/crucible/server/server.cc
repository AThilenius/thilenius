// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "cloud/utils/thrift_http_server.hh"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "scorch/cloud/crucible/server/crucible_handler.h"

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::scorch::cloud::crucible::server::CrucibleHandler;

int main(int argc, char **argv) {
  ThriftHttpServer<CrucibleHandler, ::crucible::CrucibleProcessor> http_server(
      80, "/");
  LOG(INFO) << "Starting server";
  http_server.ServeBlocking();
}
