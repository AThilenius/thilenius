// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "cloud/sentinel/Sentinel.h"
#include "cloud/sentinel/server/sentinel_handler.h"
#include "cloud/utils/thrift_http_server.hh"

using ::thilenius::cloud::utils::ThriftHttpServer;
using ::thilenius::cloud::sentinel::server::SentinelHandler;

int main(int argc, char **argv) {
  ThriftHttpServer<SentinelHandler, ::sentinel::SentinelProcessor> http_server(
      80, "/");
  LOG(INFO) << "Starting server";
  http_server.ServeBlocking();
}
