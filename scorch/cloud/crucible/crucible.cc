// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "cloud/utils/thrift_http_server.hh"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_handler.h"

int main(int argc, char **argv) {
  ::thilenius::cloud::utils::ThriftHttpServer<::thilenius::scorch::cloud::crucible::CrucibleHandler,
                   ::crucible::CrucibleProcessor> http_server (80, "/");
  LOG(INFO) << "Starting server";
  http_server.ServeBlocking();
}
