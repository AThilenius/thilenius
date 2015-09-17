// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/sentinel_client.h"

#include "base/directory.h"
#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/path.h"
#include "base/string.h"

DEFINE_string(sentinel_dir_name, ".anvil",
              "The name of the dir sentinel uses to store the key.");
DEFINE_string(sentinel_key_file_name, "sentinel_key.json",
              "The name of the sentinel key file");

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::String;

namespace thilenius {
namespace cloud {
namespace sentinel {

SentinelClient::SentinelClient(const std::string& endpoint, int port,
                               const std::string& route)
    : http_client_(endpoint, port, route) {
  http_client_.ConnectOrDie();
}

ValueOf<::sentinel::proto::Token> SentinelClient::CreateUser(
    const ::sentinel::proto::User& user_partial, const std::string& password) {
  ::sentinel::proto::User user;
  try {
    auto client = http_client_.Connect().GetOrDie();
    client->CreateUser(user, user_partial, password);
  } catch (::sentinel::proto::OperationFailure op_failure) {
    return {::sentinel::proto::Token(),
            "Sentinel remote exception: " + op_failure.user_message};
  }
  return SentinelClient::LoginUser(user.email_address, password);
}

ValueOf<::sentinel::proto::Token> SentinelClient::LoginUser(
    const std::string& email_address, const std::string& password) {
  ::sentinel::proto::Token token;
  try {
    auto client = http_client_.Connect().GetOrDie();
    client->CreateToken(token, email_address, password);
  } catch (::sentinel::proto::OperationFailure op_failure) {
    return {::sentinel::proto::Token(),
            "Sentinel remote exception: " + op_failure.user_message};
  }
  return {std::move(token)};
}

bool SentinelClient::ValidateToken(const ::sentinel::proto::Token& token) {
  try {
    auto client = http_client_.Connect().GetOrDie();
    return client->CheckToken(token);
  } catch (::sentinel::proto::OperationFailure op_failure) {
  }
  return false;
}

ValueOf<::sentinel::proto::Token> SentinelClient::LoadProjectToken(
    const std::string& project_path) {
  std::string sentinel_dir_path =
      Path::Combine(project_path, FLAGS_sentinel_dir_name);
  std::string sentinel_key_json_path =
      Path::Combine(sentinel_dir_path, FLAGS_sentinel_key_file_name);
  if (!Directory::Exists(project_path)) {
    LOG(FATAL) << "Sentinel failed to find the directory " << project_path;
  }
  if (!File::Exists(sentinel_key_json_path)) {
    LOG(FATAL) << "Sentinel failed to find a keyfile in " << project_path;
  }
  std::string json = File::ReadContentsOrDie(sentinel_key_json_path);
  return sentinel_mapper_.token_mapper.from_bson(::mongo::fromjson(json));
}

bool SentinelClient::SaveProjectToken(const ::sentinel::proto::Token& token,
                                      const std::string& project_path) {
  std::string sentinel_dir_path =
      Path::Combine(project_path, FLAGS_sentinel_dir_name);
  std::string sentinel_key_json_path =
      Path::Combine(sentinel_dir_path, FLAGS_sentinel_key_file_name);
  Directory::Create(sentinel_dir_path);
  LOG(INFO) << "Creting keyfile at " << sentinel_key_json_path;
  return File::WriteToFile(sentinel_key_json_path,
                           sentinel_mapper_.token_mapper.to_json(token));
}

ValueOf<::sentinel::proto::Token> SentinelClient::AuthorToken(
    const ::sentinel::proto::Token& authoring_token, int permission_level) {
  ::sentinel::proto::Token token;
  try {
    auto client = http_client_.Connect().GetOrDie();
    client->CreateSecondaryToken(token, authoring_token, permission_level);
  } catch (::sentinel::proto::OperationFailure op_failure) {
    return {::sentinel::proto::Token(),
            "Sentinel remote exception: " + op_failure.user_message};
  }
  return {std::move(token)};
}

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
