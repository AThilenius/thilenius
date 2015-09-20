// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_SENTINEL_CLIENT_H_
#define CLOUD_SENTINEL_SENTINEL_CLIENT_H_

#include <memory>

#include "base/value_of.hh"
#include "cloud/sentinel/Sentinel.h"
#include "cloud/sentinel/sentinel_constants.h"
#include "cloud/sentinel/sentinel_mapper.h"
#include "cloud/sentinel/sentinel_types.h"
#include "cloud/utils/thrift_http_client.hh"

using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelMapper;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace cloud {
namespace sentinel {

class SentinelClient {
 public:
  SentinelClient();

  ValueOf<void> Connect(const std::string& endpoint, int port,
                        const std::string& route);

  // Creates a new user and returns their primary token
  ValueOf<::sentinel::proto::Token> CreateUser(
      const ::sentinel::proto::User& user_partial, const std::string& password);

  // Logs an existing user in, returning a primary token
  ValueOf<::sentinel::proto::Token> LoginUser(const std::string& email_address,
                                              const std::string& password);

  // Loads a project token from JSON [default: .anvil/sentinel_key.json]
  ValueOf<::sentinel::proto::Token> LoadProjectToken(
      const std::string& project_path);

  // Saves a token to JSON [default: .anvil/sentinel_key.json]
  bool SaveProjectToken(const ::sentinel::proto::Token& token,
                        const std::string& project_path);

  // Authors a new token with permission_level one tier below the authoring
  // token
  ValueOf<::sentinel::proto::Token> AuthorToken(
      const ::sentinel::proto::Token& authoring_token, int permission_level);

  // Validates if a token is valid or not
  ValueOf<bool> ValidateToken(const ::sentinel::proto::Token& token);

 private:
  ValueOf<void> CheckConnection() const;

  typedef std::shared_ptr<ThriftHttpClient<::sentinel::proto::SentinelClient>>
      ProtoSentinelClientPtr;
  SentinelMapper sentinel_mapper_;
  ProtoSentinelClientPtr http_client_ptr_;
};

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_SENTINEL_CLIENT_H_
