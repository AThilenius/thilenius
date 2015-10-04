// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_SENTINEL_HANDLER_H_
#define CLOUD_SENTINEL_SENTINEL_HANDLER_H_

#include "cloud/sentinel/Sentinel.h"
#include "cloud/sentinel/sentinel_mapper.h"
#include "cloud/sentinel/sentinel_types.h"

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace server {

class SentinelHandler : virtual public ::sentinel::proto::SentinelIf {
 public:
  SentinelHandler();

  void CreateUser(::sentinel::proto::User& _return,
                  const ::sentinel::proto::User& new_user_partial,
                  const std::string& password);

  void CreateToken(::sentinel::proto::Token& _return, const std::string& email_address,
                   const std::string& password);

  void CreateSecondaryToken(::sentinel::proto::Token& _return,
                            const ::sentinel::proto::Token& token,
                            const int32_t permission_level);

  bool CheckToken(const ::sentinel::proto::Token& token);

  void FindUser(::sentinel::proto::User& _return, const ::sentinel::proto::Token& token,
                const ::sentinel::proto::User& user_partial);

 private:
  ::sentinel::proto::Token CreateAndSaveToken(const std::string& user_uuid,
                                       int permission_level);

  SentinelMapper mapper_;
};

}  // namespace server
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_SENTINEL_HANDLER_H_
