// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/server/sentinel_handler.h"

#include "base/crypto.h"
#include "base/guid.h"
#include "base/string.h"
#include "cloud/sentinel/sentinel_constants.h"
#include "cloud/sentinel/sentinel_model.h"
#include "third_party/mongo/client/dbclient.h"

using ::thilenius::base::Crypto;
using ::thilenius::base::Guid;
using ::thilenius::base::String;

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace server {
namespace {

void ThrowOpFailure(const std::string& user_message) {
  ::sentinel::proto::OperationFailure op_failure;
  op_failure.user_message = user_message;
  throw op_failure;
}

}  // namespace

SentinelHandler::SentinelHandler() {}

void SentinelHandler::CreateUser(
    ::sentinel::proto::User& _return,
    const ::sentinel::proto::User& new_user_partial,
    const std::string& password) {
  SentinelModel model;
  if (String::Blank(new_user_partial.first_name) ||
      String::Blank(new_user_partial.last_name) ||
      String::Blank(new_user_partial.email_address) ||
      String::Blank(password)) {
    ::sentinel::proto::OperationFailure op_failure;
    op_failure.user_message = "A required field was left blank";
    throw op_failure;
  }
  // Make sure the user is unique by email address
  ::sentinel::proto::User user;
  user.email_address = new_user_partial.email_address;
  ValueOf<SentinelMapper::SentinelEntry> duplicate_user_value =
      model.FindUser(user);
  if (duplicate_user_value.IsValid()) {
    ThrowOpFailure("A user with that email address already exists");
  }
  user.uuid = Guid::NewGuid();
  user.first_name = new_user_partial.first_name;
  user.last_name = new_user_partial.last_name;
  user.email_address = new_user_partial.email_address;
  user.permission_level =
      ::sentinel::proto::g_sentinel_constants.USER_THRESHOLD;
  // Generate a salt and password hash
  ValueOf<std::string> salt_value = Crypto::GenerateSaltBase64();
  if (!salt_value.IsValid()) {
    ThrowOpFailure("Internal server error: " + salt_value.GetError());
  }
  std::string salt = salt_value.GetOrDie();
  ValueOf<std::string> hash_value = Crypto::HashPassword(salt, password);
  if (!hash_value.IsValid()) {
    ThrowOpFailure("Internal server error: " + hash_value.GetError());
  }
  std::string hash = hash_value.GetOrDie();
  SentinelMapper::SentinelEntry entry;
  entry.user = user;
  entry.salt = salt;
  entry.hash = hash;
  if (!model.SaveSentinelEntry(entry)) {
    ThrowOpFailure("Internal server error");
  }
  _return = std::move(user);
}

void SentinelHandler::CreateToken(::sentinel::proto::Token& _return,
                                  const std::string& email_address,
                                  const std::string& password) {
  SentinelModel model;
  const std::string error_message = "Email address or password is incorrect";
  ::sentinel::proto::User user;
  user.email_address = email_address;
  // Find user
  ValueOf<SentinelMapper::SentinelEntry> existing_entry_value =
      model.FindUser(user);
  if (!existing_entry_value.IsValid()) {
    ThrowOpFailure(error_message);
  }
  SentinelMapper::SentinelEntry sentinel_entry =
      existing_entry_value.GetOrDie();
  // Check password
  ValueOf<std::string> check_password_hash_value =
      Crypto::HashPassword(sentinel_entry.salt, password);
  if (!check_password_hash_value.IsValid()) {
    LOG(ERROR) << "Crypo call failed: " << check_password_hash_value.GetError();
    ThrowOpFailure("Internal server error");
  }
  std::string check_password_hash = check_password_hash_value.GetOrDie();
  if (check_password_hash != sentinel_entry.hash) {
    ThrowOpFailure(error_message);
  }
  // User is good, create the token
  ValueOf<std::string> new_token_value = Crypto::GenerateSaltBase64();
  if (!new_token_value.IsValid()) {
    LOG(ERROR) << "Crypo call failed: " << new_token_value.GetError();
    ThrowOpFailure("Internal server error");
  }
  ::sentinel::proto::Token token;
  token.user_uuid = sentinel_entry.user.uuid;
  token.token_uuid = new_token_value.GetOrDie();
  token.permission_level = sentinel_entry.user.permission_level;
  if (!model.SaveToken(token)) {
    LOG(ERROR) << "Failed to save token to MongoDB: " << token;
    ThrowOpFailure("Internal server error");
  }
  _return = std::move(token);
}

void SentinelHandler::CreateSecondaryToken(
    ::sentinel::proto::Token& _return, const ::sentinel::proto::Token& token,
    const int32_t permission_level) {
  if (token.permission_level < ::sentinel::proto::g_sentinel_constants.USER_THRESHOLD) {
    ThrowOpFailure("Insufficient permissions");
  }
  if (permission_level >= ::sentinel::proto::g_sentinel_constants.USER_THRESHOLD) {
    ThrowOpFailure("Cannot authorize a primary token from a primary token");
  }
  if (!CheckToken(token)) {
    ThrowOpFailure("Invalid authorizing token");
  }
  // Primary token is good, create a secondary
  _return = CreateAndSaveToken(token.user_uuid, permission_level);
}

bool SentinelHandler::CheckToken(const ::sentinel::proto::Token& token) {
  SentinelModel model;
  return model.FindToken(token);
}

void SentinelHandler::FindUser(::sentinel::proto::User& _return,
                               const ::sentinel::proto::Token& token,
                               const ::sentinel::proto::User& user_partial) {
  SentinelModel model;
  if (!CheckToken(token)) {
    ThrowOpFailure("Invalid token");
  }
  ValueOf<SentinelMapper::SentinelEntry> entry_value =
      model.FindUser(user_partial);
  if (!entry_value.IsValid()) {
    ThrowOpFailure(entry_value.GetError());
  }
  SentinelMapper::SentinelEntry entry = entry_value.GetOrDie();
  if (entry.user.uuid != token.user_uuid &&
      entry.user.permission_level > token.permission_level) {
    ThrowOpFailure("You do not have required permission level");
  }
  _return = std::move(entry.user);
}

::sentinel::proto::Token SentinelHandler::CreateAndSaveToken(
    const std::string& user_uuid, int permission_level) {
  SentinelModel model;
  ::sentinel::proto::Token token;
  ValueOf<std::string> new_token_value = Crypto::GenerateSaltBase64();
  if (!new_token_value.IsValid()) {
    LOG(ERROR) << "Crypo call failed: " << new_token_value.GetError();
    ThrowOpFailure("Internal server error");
  }
  token.user_uuid = user_uuid;
  token.token_uuid = new_token_value.GetOrDie();
  token.permission_level = permission_level;
  if (!model.SaveToken(token)) {
    LOG(ERROR) << "Failed to save token to MongoDB: " << token;
    ThrowOpFailure("Internal server error");
  }
  return std::move(token);
}

}  // namespace server
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
