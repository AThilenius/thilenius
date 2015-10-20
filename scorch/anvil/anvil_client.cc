// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/anvil_client.h"

#include "base/string.h"

namespace thilenius {
namespace scorch {
namespace anvil {

AnvilClient::AnvilClient() : connected_(false) {}

ValueOf<void> AnvilClient::Connect(const std::string& blaze_ip, int blaze_port) {
  if (connected_) {
    return {"Already connected"};
  }
  blaze_ip_ = blaze_ip;
  blaze_port_ = blaze_port;
  // Connect blaze
  LOG(INFO) << "Connecting to Blaze";
  standard_client_ptr_ = ProtoBlazeClientPtr(
      new ThriftStandardClient<::blaze::proto::BlazeCommandProcessorClient>(
          blaze_ip, blaze_port));
  auto connection = standard_client_ptr_->Connect();
  if (!connection.IsValid()) {
    return {connection.GetError()};
  }
  connected_ = true;
  return {};
}

ValueOf<::blaze::proto::BlazeResponseWrapper> AnvilClient::Process(
    const ::blaze::proto::BlazeRequestWrapper& request) {
  if (!connected_) {
    return {::blaze::proto::BlazeResponseWrapper(),
            "You must be connected before making API calls"};
  }
  ::blaze::proto::BlazeResponseWrapper response;
  try {
    standard_client_ptr_->ConnectOrDie()->Process(response, request);
  } catch (::blaze::proto::OperationFailure op_failure) {
    return {::blaze::proto::BlazeResponseWrapper(),
            StrCat("Blaze remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {::blaze::proto::BlazeResponseWrapper(),
            "In Process, Blaze server threw an unhandled exception."};
  }
  return std::move(response);
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
