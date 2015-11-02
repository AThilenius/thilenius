// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/blaze/blaze_handler.h"

#include <gflags/gflags.h>

#include "base/http.h"
#include "base/json.h"
#include "base/log.h"
#include "base/string.h"
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

using ::thilenius::base::Http;
using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelClient;

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
  // Args: world_id, command, message
  if (command_name != "action_target_invoke") {
    ThrowOpFailure("Invalid command type");
  }
  if (args_json.size() < 3) {
    ThrowOpFailure("Expected 3 args in args_json");
  }
  int world = -1;
  std::string command;
  ::nlohmann::json message;
  try {
    world = std::stoi(args_json[0]);
    command = args_json[1];
    message = ::nlohmann::json::parse(args_json[2]);
  } catch (...) {
    ThrowOpFailure("Malformed JSON");
  }
  ::nlohmann::json json = {
      {"user_uuid", token.user_uuid},
      {"minecraft_username", minecraft_account.minecraft_username},
      {"dimension", world},
      {"target", command},
      {"json", message}};
  ValueOf<std::string> flame_response_value = Http::PostContent(
      StrCat(FLAGS_flame_ip, ":", std::to_string(FLAGS_flame_port)),
      json.dump());
  if (!flame_response_value.IsValid()) {
    LOG(ERROR) << "Failed to execute Flame command: "
               << flame_response_value.GetError();
    ThrowOpFailure("Internal server error");
  }
  try {
    ::nlohmann::json response =
        ::nlohmann::json::parse(flame_response_value.GetOrDie());
    if (response["is_exception"].get<bool>()) {
      ThrowOpFailure(response["user_message"].get<std::string>());
    }
    _return = response["json"].dump();
  } catch (...) {
    LOG(ERROR) << "Failed to Flame response JSON!";
    ThrowOpFailure("Internal server error");
  }
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
