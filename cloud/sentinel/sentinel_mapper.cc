// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/sentinel_mapper.h"

#include "base/log.h"

namespace thilenius {
namespace cloud {
namespace sentinel {

SentinelMapper::SentinelMapper() {
  // User
  user_mapper.add_field("uuid", &::sentinel::User::uuid);
  user_mapper.add_field("first_name", &::sentinel::User::first_name);
  user_mapper.add_field("last_name", &::sentinel::User::last_name);
  user_mapper.add_field("email_address", &::sentinel::User::email_address);
  user_mapper.add_field("permission_level",
                        &::sentinel::User::permission_level);

  // Token
  token_mapper.add_field("user_uuid", &::sentinel::Token::user_uuid);
  token_mapper.add_field("token_uuid", &::sentinel::Token::token_uuid);
  token_mapper.add_field("permission_level",
                         &::sentinel::Token::permission_level);

  // Entry
  entry_mapper.add_field("user", &SentinelEntry::user, user_mapper);
  entry_mapper.add_field("salt", &SentinelEntry::salt);
  entry_mapper.add_field("hash", &SentinelEntry::hash);
}

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
