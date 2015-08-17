// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_UTILS_THRIFT_HTTP_SERVER_H_
#define CLOUD_UTILS_THRIFT_HTTP_SERVER_H_

#include <memory>
#include <string>

#include "base/log.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/server/TSimpleServer.h"
#include "third_party/thrift/transport/THttpServer.h"
#include "third_party/thrift/transport/TServerSocket.h"

namespace thilenius {
namespace cloud {
namespace utils {

// A header only, simple HTTP server in Thrift
template <typename handler_t, typename procesor_t>
class ThriftHttpServer {
 public:
  // TODO(athilenius): Use perfect forwarding for hander constructor parameter
  // pass though
  ThriftHttpServer(int port, const std::string& route)
      : handler_(new handler_t()),
        processor_(new procesor_t(handler_)),
        protocol_factory_(
            new ::apache::thrift::protocol::TJSONProtocolFactory()),
        server_transport_(new ::apache::thrift::transport::TServerSocket(port)),
        transport_factory_(
            new ::apache::thrift::transport::THttpServerTransportFactory()),
        server_(processor_, server_transport_, transport_factory_,
                protocol_factory_) {}

  void ServeBlocking() { server_.serve(); }

 private:
  ::boost::shared_ptr<handler_t> handler_;
  ::boost::shared_ptr<::apache::thrift::TProcessor> processor_;
  ::boost::shared_ptr<::apache::thrift::protocol::TProtocolFactory>
      protocol_factory_;
  ::boost::shared_ptr<::apache::thrift::transport::TServerTransport>
      server_transport_;
  ::boost::shared_ptr<::apache::thrift::transport::TTransportFactory>
      transport_factory_;
  ::apache::thrift::server::TSimpleServer server_;
};  // class ThriftHttpClient

}  // namespace utils
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_UTILS_THRIFT_HTTP_SERVER_H_
