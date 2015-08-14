// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_handler.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/server/TSimpleServer.h"
#include "third_party/thrift/transport/THttpServer.h"
#include "third_party/thrift/transport/TServerSocket.h"
#include "third_party/thrift/transport/TBufferTransports.h"

#include "third_party/thrift/concurrency/ThreadManager.h"
#include "third_party/thrift/concurrency/PosixThreadFactory.h"
#include "third_party/thrift/protocol/TBinaryProtocol.h"
#include "third_party/thrift/server/TSimpleServer.h"
#include "third_party/thrift/server/TThreadPoolServer.h"
#include "third_party/thrift/server/TThreadedServer.h"
#include "third_party/thrift/transport/TServerSocket.h"
#include "third_party/thrift/transport/TTransportUtils.h"

int main(int argc, char **argv) {
  using namespace ::apache::thrift;
  using namespace ::apache::thrift::concurrency;
  using namespace ::apache::thrift::protocol;
  using namespace ::apache::thrift::transport;
  using namespace ::apache::thrift::server;
  using ::thilenius::scorch::cloud::crucible::CrucibleHandler;
  using ::boost::shared_ptr;

  shared_ptr<CrucibleHandler> handler(new CrucibleHandler());
  shared_ptr<TProcessor> processor(
      new ::crucible::CrucibleProcessor(handler));

  shared_ptr<TProtocolFactory> protocolFactory(new TJSONProtocolFactory());
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(80));
  shared_ptr<TTransportFactory> transportFactory(
      new THttpServerTransportFactory());

  TSimpleServer server(processor, serverTransport, transportFactory,
                       protocolFactory);
}
