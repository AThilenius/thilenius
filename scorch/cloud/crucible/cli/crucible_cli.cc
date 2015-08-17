// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/server/TSimpleServer.h"
#include "third_party/thrift/transport/THttpServer.h"
#include "third_party/thrift/transport/TServerSocket.h"

int main(int argc, char **argv) {
  using namespace ::apache::thrift;
  using namespace ::apache::thrift::concurrency;
  using namespace ::apache::thrift::protocol;
  using namespace ::apache::thrift::transport;
  using namespace ::apache::thrift::server;
  using ::boost::shared_ptr;
  // Init Google FLags
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  // Create and launch a HTTP Thrift service host
  //LOG(INFO) << "Creating HTTP service stack for thrift...";
  //shared_ptr<CrucibleHandler> handler(new CrucibleHandler());
  //shared_ptr<TProcessor> processor(
      //new ::crucible::CrucibleProcessor(handler));
  //shared_ptr<TProtocolFactory> protocolFactory(new TJSONProtocolFactory());
  //shared_ptr<TServerTransport> serverTransport(new TServerSocket(80));
  //shared_ptr<TTransportFactory> transportFactory(
      //new THttpServerTransportFactory());
  //TSimpleServer server(processor, serverTransport, transportFactory,
                       //protocolFactory);
  //LOG(INFO) << "Starting server";
  //server.serve();
}
