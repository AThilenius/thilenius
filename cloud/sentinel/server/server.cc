// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <gflags/gflags.h>

#include "base/log.h"
#include "cloud/sentinel/gen-cpp2/Sentinel.h"
#include "cloud/sentinel/server/sentinel_handler.h"

DEFINE_int32(server_port, 2100, "The port for the Sentinel server.");

using ::thilenius::cloud::sentinel::server::SentinelHandler;

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  std::shared_ptr<SentinelHandler> ptr(new SentinelHandler());
  ::apache::thrift::ThriftServer* s = new ::apache::thrift::ThriftServer();
  s->setInterface(ptr);
  s->setPort(8080);
  s->serve();
}
