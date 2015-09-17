// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/sentinel_user.h"

#include "base/directory.h"
#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/path.h"
#include "base/string.h"
#include "cloud/sentinel/Sentinel.h"
#include "cloud/sentinel/sentinel_constants.h"
#include "cloud/sentinel/sentinel_mapper.h"
#include "cloud/sentinel/sentinel_types.h"
#include "cloud/utils/thrift_http_client.hh"

DEFINE_string(sentinel_endpoint, "127.0.0.1",
              "The endpoint for the Sentinel server.");
DEFINE_string(sentinel_endpoint_route, "/",
              "The http route for the Sentinel server.");
DEFINE_int32(sentinel_endpoint_port, 80, "The port for the Sentinel server.");
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
namespace {

::thilenius::cloud::sentinel::SentinelMapper sentinel_mapper_;
::thilenius::cloud::utils::ThriftHttpClient<::sentinel::SentinelClient>
    http_client_(FLAGS_sentinel_endpoint, FLAGS_sentinel_endpoint_port,
                 FLAGS_sentinel_endpoint_route);

}  // namespace

SentinelUser::SentinelUser(const ::sentinel::Token& token)
    : primary_token_(token) {}

void SentinelUser::CheckConnectionOrDie() {
  http_client_ =
      ::thilenius::cloud::utils::ThriftHttpClient<::sentinel::SentinelClient>(
          FLAGS_sentinel_endpoint, FLAGS_sentinel_endpoint_port,
          FLAGS_sentinel_endpoint_route);
  LOG(INFO) << "Connection to Sentinel at " << FLAGS_sentinel_endpoint << ":"
            << FLAGS_sentinel_endpoint_port;
  http_client_.ConnectOrDie();
}

ValueOf<SentinelUser> SentinelUser::Create(const ::sentinel::User& user_partial,
                                           const std::string& password) {
  ::sentinel::User user;
  try {
    auto client = http_client_.Connect().GetOrDie();
    client->CreateUser(user, user_partial, password);
  } catch (::sentinel::OperationFailure op_failure) {
    return {SentinelUser(::sentinel::Token()),
            "Sentinel remote exception: " + op_failure.user_message};
  }
  return SentinelUser::Login(user.email_address, password);
}

ValueOf<SentinelUser> SentinelUser::Login(const std::string& email_address,
                                          const std::string& password) {
  ::sentinel::Token token;
  try {
    auto client = http_client_.Connect().GetOrDie();
    client->CreateToken(token, email_address, password);
  } catch (::sentinel::OperationFailure op_failure) {
    return {SentinelUser(::sentinel::Token()),
            "Sentinel remote exception: " + op_failure.user_message};
  }
  SentinelUser sentinel_user(token);
  return {std::move(sentinel_user)};
}

bool SentinelUser::CheckToken(const ::sentinel::Token& token) {
  try {
    auto client = http_client_.Connect().GetOrDie();
    return client->CheckToken(token);
  } catch (::sentinel::OperationFailure op_failure) {
    return false;
  }
}

ValueOf<::sentinel::Token> SentinelUser::LoadToken(
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

bool SentinelUser::SaveToken(const ::sentinel::Token& token,
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

ValueOf<::sentinel::Token> SentinelUser::CreateToken(int permission_level) {
  ::sentinel::Token token;
  try {
    auto client = http_client_.Connect().GetOrDie();
    client->CreateSecondaryToken(token, primary_token_, permission_level);
  } catch (::sentinel::OperationFailure op_failure) {
    return {::sentinel::Token(),
            "Sentinel remote exception: " + op_failure.user_message};
  }
  return {std::move(token)};
}

::sentinel::Token SentinelUser::PrimaryToken() { return primary_token_; }

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
