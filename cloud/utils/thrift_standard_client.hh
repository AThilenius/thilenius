// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_UTILS_THRIFT_STANDARD_CLIENT_H_
#define CLOUD_UTILS_THRIFT_STANDARD_CLIENT_H_

#include <memory>
#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "base/log.h"
#include "base/value_of.hh"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace cloud {
namespace utils {

// A header only, simple standard client in Thrift
template <typename T>
class ThriftStandardClient {
 public:
  typedef std::shared_ptr<T> ClientPtr;
  ThriftStandardClient(const std::string& ip, int port)
      : connected_(false),
        socket_(new ::apache::thrift::transport::TSocket(ip, port)),
        transport_(
            new ::apache::thrift::transport::TFramedTransport(socket_)),
        protocol_(new ::apache::thrift::protocol::TBinaryProtocol(transport_)),
        client_(new T(protocol_)) {}

  ~ThriftStandardClient() { transport_->close(); }

  ClientPtr ConnectOrDie() {
    if (!connected_) {
      try {
        transport_->open();
        connected_ = true;
      } catch (::apache::thrift::TException& tx) {
        LOG(FATAL) << "Failed to connect to Thrift Standard (non-HTTP) Server!";
      }
    }
    return client_;
  }

  ValueOf<ClientPtr> Connect() {
    if (!connected_) {
      try {
        transport_->open();
        connected_ = true;
      } catch (::apache::thrift::TException& tx) {
        return {nullptr,
                "Failed to connect to Thrift Standard (non-HTTP) Server!"};
      }
    }
    ClientPtr client = client_;
    return {std::move(client)};
  }

 private:
  bool connected_;
  ::boost::shared_ptr<::apache::thrift::transport::TTransport> socket_;
  ::boost::shared_ptr<::apache::thrift::transport::TTransport> transport_;
  ::boost::shared_ptr<::apache::thrift::protocol::TProtocol> protocol_;
  std::shared_ptr<T> client_;
};  // class ThriftStandardClient

}  // namespace utils
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_UTILS_THRIFT_STANDARD_CLIENT_H_
