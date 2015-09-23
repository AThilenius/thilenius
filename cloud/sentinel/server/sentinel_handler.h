// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_SENTINEL_HANDLER_H_
#define CLOUD_SENTINEL_SENTINEL_HANDLER_H_

#include "cloud/sentinel/gen-cpp2/Sentinel.h"
#include "cloud/sentinel/gen-cpp2/sentinel_constants.h"
#include "cloud/sentinel/gen-cpp2/sentinel_types.h"
#include "cloud/sentinel/sentinel_mapper.h"
#include "cloud/sentinel/sentinel_model.h"
#include "third_party/mongo/client/dbclient.h"

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace server {

class SentinelHandler : virtual public ::sentinel::proto::SentinelSvIf {
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

  ::mongo::DBClientConnection mongo_connection_;
  ::sentinel::proto::sentinelConstants sentinel_constants_;
  SentinelModel model_;
  SentinelMapper mapper_;
};

}  // namespace server
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_SENTINEL_HANDLER_H_
