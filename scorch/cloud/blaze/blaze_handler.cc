// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/blaze/blaze_handler.h"

#include <gflags/gflags.h>

#include "base/log.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_client.h"
#include "cloud/utils/thrift_standard_client.hh"
#include "scorch/cloud/blaze/BlazeCommandProcessor.h"
#include "scorch/cloud/blaze/blaze_model.h"

DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");
DEFINE_string(flame_ip, "192.168.59.3", "The Flame instance ip address.");
DEFINE_int32(flame_port, 2500, "The Flame instance port number.");

using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelClient;
using ::thilenius::cloud::utils::ThriftStandardClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

BlazeHandler::BlazeHandler() {}

void BlazeHandler::ProcessBlazeRequest(
    std::string& _return, const ::sentinel::proto::Token& token,
    const std::string& command_name,
    const std::vector<std::string>& args_json) {
  AuthenticateOrThrow(token);
  BlazeModel blaze_model;
  auto mc_account_value =
      blaze_model.FindMinecraftAccountByUserUuid(token.user_uuid);
  if (!mc_account_value.IsValid()) {
    ThrowOpFailure(mc_account_value.GetError());
  }
  ::blaze::proto::MinecraftAccount minecraft_account =
      mc_account_value.GetOrDie();
  // Connect to Blaze Command Processor (aka Flame)
  ThriftStandardClient<::blaze::proto::BlazeCommandProcessorClient> http_client(
      FLAGS_flame_ip, FLAGS_flame_port);
  auto connection = http_client.Connect();
  if (!connection.IsValid()) {
    LOG(ERROR) << "Failed to connect to Flame: " << connection.GetError();
    ThrowOpFailure("Internal server failure");
  }
  auto flame_connection = connection.GetOrDie();
  // Override the user uuid and minecraft username
  ::blaze::proto::BlazeRequestWrapper request;
  request.user_uuid = token.user_uuid;
  request.minecraft_username = minecraft_account.minecraft_username;
  request.command_name = command_name;
  request.args_json = args_json;
  ::blaze::proto::BlazeResponseWrapper response;
  flame_connection->Process(response, request);
  if (response.is_exception) {
    ThrowOpFailure(response.json);
  }
  _return = std::move(response.json);
}

void BlazeHandler::SetMinecraftAccount(
    const ::sentinel::proto::Token& token,
    const ::blaze::proto::MinecraftAccount& minecraft_account) {
  AuthenticateOrThrow(token);
  BlazeModel blaze_model;
  ::blaze::proto::MinecraftAccount new_account;
  new_account.user_uuid = token.user_uuid;
  new_account.minecraft_username = minecraft_account.minecraft_username;
  if (!blaze_model.SaveMinecraftAccount(new_account)) {
    ThrowOpFailure("Failed to save account");
  }
}

void BlazeHandler::GetMinecraftAccount(
    ::blaze::proto::MinecraftAccount& _return,
    const ::sentinel::proto::Token& token) {
  AuthenticateOrThrow(token);
  BlazeModel blaze_model;
  ValueOf<::blaze::proto::MinecraftAccount> account_value =
      blaze_model.FindMinecraftAccountByUserUuid(token.user_uuid);
  if (!account_value.IsValid()) {
    ThrowOpFailure("No account on file");
  }
  _return = account_value.GetOrDie();
}

void BlazeHandler::ThrowOpFailure(const std::string& message) {
  ::blaze::proto::OperationFailure op_failure;
  op_failure.user_message = message;
  LOG(WARNING) << "OP Failure: " << message;
  throw op_failure;
}

void BlazeHandler::AuthenticateOrThrow(const ::sentinel::proto::Token& token) {
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route);
  // Authenticate
  if (!sentinel_client.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
