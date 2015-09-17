// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_SENTINEL_USER_H_
#define CLOUD_SENTINEL_SENTINEL_USER_H_

#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_types.h"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace cloud {
namespace sentinel {

class SentinelUser {
 public:
  static void CheckConnectionOrDie();

  static ValueOf<SentinelUser> Create(const ::sentinel::User& user_partial,
                                      const std::string& password);

  static ValueOf<SentinelUser> Login(const std::string& email_address,
                                     const std::string& password);

  static bool CheckToken(const ::sentinel::Token& token);

  static ValueOf<::sentinel::Token> LoadToken(const std::string& project_path);

  bool SaveToken(const ::sentinel::Token& token,
                 const std::string& project_path);

  ValueOf<::sentinel::Token> CreateToken(int permission_level);

  ::sentinel::Token PrimaryToken();

  std::string UserID();

 private:
  SentinelUser(const ::sentinel::Token& token);

  const ::sentinel::Token primary_token_;
};

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_SENTINEL_USER_H_
