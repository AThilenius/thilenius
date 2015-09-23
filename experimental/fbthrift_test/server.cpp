// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>

#include "gen-cpp2/Calculator.h"  // This is included from generated code
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp/protocol/TJSONProtocol.h>
#include <thrift/lib/cpp/transport/THttpServer.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace example::cpp2;

// The thrift has generated service interface with name CalculatorSvIf
// At server side we have to implement this interface
class CalculatorSvc : public CalculatorSvIf {
 public:
  virtual ~CalculatorSvc() {}
  // We have to implement async_tm_add to implement the add function
  // of the Calculator service which we defined in calculator.thrift file
  void async_tm_add(
      std::unique_ptr<apache::thrift::HandlerCallback<int64_t>> callback,
      int32_t num1, int32_t num2) {
    cout << "Got async request " << num1 << " + " << num2 << endl;
    callback->result(num1 + num2);  // return the result via callback
  }
};

void httpHandler(apache::thrift::async::TEventBase* ev,
                 std::shared_ptr<apache::thrift::async::TAsyncTransport> tr,
                 std::unique_ptr<folly::IOBuf> buf) {
  std::cout << "Got http request" << std::endl;
  std::string resp =
      "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
      "11\r\n\r\nHello World";
  tr->write(nullptr, resp.c_str(), resp.size());
}

int main(int argc, char** argv) {
  // To create a server, we need to first create server handler object.
  // The server handler object contains the implementation of the service.
  std::shared_ptr<CalculatorSvc> ptr(new CalculatorSvc());
  // Create a thrift server
  ThriftServer* s = new ThriftServer();

  // std::shared_ptr<TTransportFactory> transport_factory (new
  // THttpServerTransportFactory());
  // std::shared_ptr<TProtocolFactory> protocol_factory (new
  // TJSONProtocolFactory());
  // s->setProtocolFactory(protocol_factory);
  // s->setTransportFactory(transport_factory);

  s->setGetHandler(httpHandler);

  // Set server handler object
  s->setInterface(ptr);
  // Set the server port
  s->setPort(80);
  // Start the server to serve!!
  s->serve();

  return 0;
}
