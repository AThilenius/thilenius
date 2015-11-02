// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/blaze/blaze_client.h"

#include <gflags/gflags.h>

#include "base/json.h"
#include "base/log.h"
#include "base/string.h"
#include "base/types.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

BlazeClient::BlazeClient() : token_(nullptr) {}

ValueOf<void> BlazeClient::Connect(const std::string& blaze_ip, int blaze_port,
                                   const std::string& blaze_route,
                                   const std::string& sentinel_ip,
                                   int sentinel_port,
                                   const std::string& sentinel_route,
                                   const std::string& project_path) {
  if (connected_) {
    return {"Already connected"};
  }
  blaze_ip_ = blaze_ip;
  blaze_port_ = blaze_port;
  blaze_route_ = blaze_route;
  project_path_ = project_path;
  sentinel_ip_ = sentinel_ip;
  sentinel_port_ = sentinel_port;
  sentinel_route_ = sentinel_route;
  // Connect blaze
  LOG(INFO) << "Connection to Blaze";
  http_client_ptr_ =
      ProtoBlazeClientPtr(new ThriftHttpClient<::blaze::proto::BlazeClient>(
          blaze_ip, blaze_port, blaze_route));
  auto connection = http_client_ptr_->Connect();
  if (!connection.IsValid()) {
    return {connection.GetError()};
  }
  // Connect sentinel
  LOG(INFO) << "Connection to Sentinel";
  auto sentinel_connection =
      sentinel_client_.Connect(sentinel_ip_, sentinel_port_, sentinel_route_);
  if (!sentinel_connection.IsValid()) {
    return {sentinel_connection.GetError()};
  }
  connected_ = true;
  return {};
}

ValueOf<void> BlazeClient::SetMinecraftAccount(
    const std::string& minecraft_username) {
  ValueOf<::sentinel::proto::Token> token_value = LoadOrCreateToken();
  if (!token_value.IsValid()) {
    return std::move(token_value.GetError());
  }
  ::sentinel::proto::Token token = token_value.GetOrDie();
  ::blaze::proto::MinecraftAccount minecraft_account;
  minecraft_account.user_uuid = token.user_uuid;
  minecraft_account.minecraft_username = minecraft_username;
  try {
    http_client_ptr_->ConnectOrDie()->SetMinecraftAccount(token,
                                                          minecraft_account);
  } catch (::blaze::proto::OperationFailure op_failure) {
    return {StrCat("Blaze remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {
        "In SetMinecraftAccount, Blaze server threw an unhandled "
        "exception."};
  }
  return {};
}

ValueOf<::nlohmann::json> BlazeClient::Process(
    const std::string& command_name,
    const std::vector<::nlohmann::json>& args) {
  ValueOf<::sentinel::proto::Token> token_value = LoadOrCreateToken();
  if (!token_value.IsValid()) {
    return {::nlohmann::json(), std::move(token_value.GetError())};
  }
  ::sentinel::proto::Token token = token_value.GetOrDie();
  std::string response_json_str;
  // JSON to string vector
  std::vector<std::string> string_json_args;
  for (const auto& json : args) {
    string_json_args.emplace_back(json.dump());
  }
  try {
    http_client_ptr_->ConnectOrDie()->ProcessBlazeRequest(
        response_json_str, token, command_name, string_json_args);
    ::nlohmann::json response_json(response_json_str);
    return std::move(response_json);
  } catch (::blaze::proto::OperationFailure op_failure) {
    return {StrCat("Blaze remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {
        "In Process, Blaze server threw an unhandled exception or returned "
        "invalid JSON"};
  }
}

ValueOf<::sentinel::proto::Token> BlazeClient::LoadOrCreateToken() {
  ::sentinel::proto::Token token;
  if (token_) {
    token = *token_;
    return std::move(token);
  }
  ValueOf<::sentinel::proto::Token> token_value =
      sentinel_client_.LoadProjectToken(project_path_);
  if (!token_value.IsValid()) {
    return token_value;
  }
  token = token_value.GetOrDie();
  *token_ = token;
  // Save the token as well
  sentinel_client_.SaveProjectToken(token, project_path_);
  return std::move(token);
}

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
