// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/fiber/fiber_client.h"

#include "base/log.h"

namespace thilenius {
namespace cloud {
namespace fiber {

FiberClient::FiberClient()
    : http_client_ptr_(ProtoFiberClientPtr(nullptr)), connected_(false) {}

ValueOf<void> FiberClient::Connect(const std::string& fiber_ip, int fiber_port,
                                   const std::string& fiber_route,
                                   const std::string& sentinel_ip,
                                   int sentinel_port,
                                   const std::string& sentinel_route) {
  if (connected_) {
    return {"Already connected"};
  }
  // Connect fiber
  LOG(INFO) << "Connection to Fiber";
  http_client_ptr_ =
      ProtoFiberClientPtr(new ThriftHttpClient<::fiber::proto::FiberClient>(
          fiber_ip, fiber_port, fiber_route));
  auto connection = http_client_ptr_->Connect();
  if (!connection.IsValid()) {
    return {connection.GetError()};
  }
  // Connect sentinel
  LOG(INFO) << "Connection to Sentinel";
  auto sentinel_connection =
      sentinel_client_.Connect(sentinel_ip, sentinel_port, sentinel_route);
  if (!sentinel_connection.IsValid()) {
    return {sentinel_connection.GetError()};
  }
  connected_ = true;
  return {};
}

ValueOf<::fiber::proto::Cord> FiberClient::CreateCord(
    const std::string& project_path, const std::string& name) {
  ValueOf<::sentinel::proto::Token> token_value =
      sentinel_client_.LoadProjectTokenOrLoginFromCin(project_path);
  if (!token_value.IsValid()) {
    return {::fiber::proto::Cord(), token_value.GetError()};
  }
  ::sentinel::proto::Token token = token_value.GetOrDie();

  auto client = http_client_ptr_->ConnectOrDie();
  ::fiber::proto::Cord cord;
  try {
    client->CreateCord(cord, token, name);
    return std::move(cord);
  } catch (::fiber::proto::OperationFailure op_failure) {
    return {::fiber::proto::Cord(),
            "Fiber remote exception: " + op_failure.user_message};
  }
}

ValueOf<::fiber::proto::Cord> FiberClient::GetCord(
    const std::string& cord_uuid) {
  auto client = http_client_ptr_->ConnectOrDie();
  ::fiber::proto::Cord cord;
  try {
    client->GetCord(cord, cord_uuid);
    return std::move(cord);
  } catch (::fiber::proto::OperationFailure op_failure) {
    return {::fiber::proto::Cord(),
            "Fiber remote exception: " + op_failure.user_message};
  }
}

ValueOf<void> FiberClient::WriteCord(
    const ::fiber::proto::Cord& cord,
    const std::vector<::fiber::proto::Grain>& grains) {
  auto client = http_client_ptr_->ConnectOrDie();
  try {
    client->WriteCord(cord, grains);
    return {};
  } catch (::fiber::proto::OperationFailure op_failure) {
    return {"Fiber remote exception: " + op_failure.user_message};
  } catch (::fiber::proto::EndOfCord end_of_cord) {
    return {"EndOfCord"};
  }
}

ValueOf<void> FiberClient::CloseCord(const ::fiber::proto::Cord& cord) {
  auto client = http_client_ptr_->ConnectOrDie();
  try {
    client->CloseCord(cord);
    return {};
  } catch (::fiber::proto::OperationFailure op_failure) {
    return {"Fiber remote exception: " + op_failure.user_message};
  } catch (::fiber::proto::EndOfCord end_of_cord) {
    return {"EndOfCord"};
  }
}

ValueOf<std::vector<::fiber::proto::Grain>> FiberClient::ReadCordGTEIndex(
    const ::fiber::proto::Cord& cord, int index) {
  auto client = http_client_ptr_->ConnectOrDie();
  std::vector<::fiber::proto::Grain> grains;
  try {
    client->ReadCordGTEIndex(grains, cord, index);
    return std::move(grains);
  } catch (::fiber::proto::OperationFailure op_failure) {
    return {std::vector<::fiber::proto::Grain>(),
            "Fiber remote exception: " + op_failure.user_message};
  } catch (::fiber::proto::EndOfCord end_of_cord) {
    return {std::vector<::fiber::proto::Grain>(), "EndOfCord"};
  }
}

}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius
