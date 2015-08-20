// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_UTILS_THRIFT_HTTP_CLIENT_H_
#define CLOUD_UTILS_THRIFT_HTTP_CLIENT_H_

#include <memory>
#include <string>

#include "base/log.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/transport/THttpClient.h"
#include "third_party/thrift/transport/TTransportUtils.h"

namespace thilenius {
namespace cloud {
namespace utils {

// A header only, simple HTTP client in Thrift
template <typename T>
class ThriftHttpClient {
 public:
  typedef std::shared_ptr<T> ClientPtr;
  ThriftHttpClient(const std::string& ip, int port, const std::string& route)
      : connected_(false),
        transport_(
            new ::apache::thrift::transport::THttpClient(ip, port, route)),
        protocol_(new ::apache::thrift::protocol::TJSONProtocol(transport_)),
        client_(new T(protocol_)) {}

  ~ThriftHttpClient() { transport_->close(); }

  ClientPtr ConnectOrDie() {
    if (!connected_) {
      try {
        transport_->open();
        connected_ = true;
      } catch (::apache::thrift::TException& tx) {
        LOG(FATAL) << "Failed to connect to Thrift HTTP Server!";
      }
    }
    return client_;
  }

 private:
  bool connected_;
  ::boost::shared_ptr<::apache::thrift::transport::TTransport> transport_;
  ::boost::shared_ptr<::apache::thrift::protocol::TProtocol> protocol_;
  std::shared_ptr<T> client_;
};  // class ThriftHttpClient

}  // namespace utils
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_UTILS_THRIFT_HTTP_CLIENT_H_
