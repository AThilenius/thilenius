// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/server/sentinel_handler.h"

#include <gflags/gflags.h>

#include "base/crypto.h"
#include "base/guid.h"
#include "base/string.h"

DEFINE_string(mongo_ip, "192.168.59.103", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");
DEFINE_string(mongo_user_table, "sentinel.users",
              "The MongoDB database and table name for user entries.");
DEFINE_string(mongo_token_table, "sentinel.tokens",
              "The MongoDB database and table name for tokens.");

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

SentinelHandler::SentinelHandler()
    : model_(mongo_connection_, FLAGS_mongo_user_table,
             FLAGS_mongo_token_table) {
  try {
    LOG(INFO) << "Connecting to MongoDB at " << FLAGS_mongo_ip << ":"
              << FLAGS_mongo_port;
    mongo_connection_.connect(StrCat(FLAGS_mongo_ip, ":", FLAGS_mongo_port));
  } catch (const mongo::DBException& e) {
    LOG(FATAL) << "MongoDB Driver failed to connect to " << FLAGS_mongo_ip
               << ":" << FLAGS_mongo_port;
  }
}

void SentinelHandler::CreateUser(::sentinel::proto::User& _return,
                                 const ::sentinel::proto::User& new_user_partial,
                                 const std::string& password) {
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
      model_.FindUser(user);
  if (duplicate_user_value.IsValid()) {
    ThrowOpFailure("A user with that email address already exists");
  }
  user.uuid = Guid::NewGuid();
  user.first_name = new_user_partial.first_name;
  user.last_name = new_user_partial.last_name;
  user.email_address = new_user_partial.email_address;
  user.permission_level = sentinel_constants_.USER_THRESHOLD;
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
  if (!model_.SaveSentinelEntry(entry)) {
    ThrowOpFailure("Internal server error");
  }
  _return = std::move(user);
}

void SentinelHandler::CreateToken(::sentinel::proto::Token& _return,
                                  const std::string& email_address,
                                  const std::string& password) {
  const std::string error_message = "Email address or password is incorrect";
  ::sentinel::proto::User user;
  user.email_address = email_address;
  // Find user
  ValueOf<SentinelMapper::SentinelEntry> existing_entry_value =
      model_.FindUser(user);
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
  if (!model_.SaveToken(token)) {
    LOG(ERROR) << "Failed to save token to MongoDB: " << token;
    ThrowOpFailure("Internal server error");
  }
  _return = std::move(token);
}

void SentinelHandler::CreateSecondaryToken(::sentinel::proto::Token& _return,
                                           const ::sentinel::proto::Token& token,
                                           const int32_t permission_level) {
  if (token.permission_level < sentinel_constants_.USER_THRESHOLD) {
    ThrowOpFailure("Insufficient permissions");
  }
  if (permission_level >= sentinel_constants_.USER_THRESHOLD) {
    ThrowOpFailure("Cannot authorize a primary token from a primary token");
  }
  if (!CheckToken(token)) {
    ThrowOpFailure("Invalid authorizing token");
  }
  // Primary token is good, create a secondary
  _return = CreateAndSaveToken(token.user_uuid, permission_level);
}

bool SentinelHandler::CheckToken(const ::sentinel::proto::Token& token) {
  return model_.FindToken(token);
}

void SentinelHandler::FindUser(::sentinel::proto::User& _return,
                               const ::sentinel::proto::Token& token,
                               const ::sentinel::proto::User& user_partial) {}

::sentinel::proto::Token SentinelHandler::CreateAndSaveToken(
    const std::string& user_uuid, int permission_level) {
  ::sentinel::proto::Token token;
  ValueOf<std::string> new_token_value = Crypto::GenerateSaltBase64();
  if (!new_token_value.IsValid()) {
    LOG(ERROR) << "Crypo call failed: " << new_token_value.GetError();
    ThrowOpFailure("Internal server error");
  }
  token.user_uuid = user_uuid;
  token.token_uuid = new_token_value.GetOrDie();
  token.permission_level = permission_level;
  if (!model_.SaveToken(token)) {
    LOG(ERROR) << "Failed to save token to MongoDB: " << token;
    ThrowOpFailure("Internal server error");
  }
  return std::move(token);
}

}  // namespace server
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
