// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_CLIENT_H_
#define SCORCH_ANVIL_ANVIL_CLIENT_H_

#include <string>

#include "cloud/utils/thrift_standard_client.hh"
#include "scorch/anvil/BlazeCommandProcessor.h"
#include "scorch/anvil/blaze_types.h"

using ::thilenius::cloud::utils::ThriftStandardClient;

namespace thilenius {
namespace scorch {
namespace anvil {

class AnvilClient {
 public:
  AnvilClient();

  // Connects to the specified Blaze instance.
  ValueOf<void> Connect(const std::string& blaze_ip, int blaze_port);

  // HACK
  ValueOf<::blaze::proto::BlazeResponseWrapper> Process(
      const ::blaze::proto::BlazeRequestWrapper& request);

 private:
  typedef std::shared_ptr<ThriftStandardClient<
      ::blaze::proto::BlazeCommandProcessorClient>> ProtoBlazeClientPtr;

  ProtoBlazeClientPtr standard_client_ptr_;
  bool connected_;
  int blaze_port_;
  std::string blaze_ip_;
};

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_CLIENT_H_
