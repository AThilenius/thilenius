// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BLAZE_BLAZE_CLIENT_H_
#define SCORCH_CLOUD_BLAZE_BLAZE_CLIENT_H_

#include <vector>

#include "base/json.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_client.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/blaze/Blaze.h"
#include "scorch/cloud/blaze/blaze_constants.h"
#include "scorch/cloud/blaze/blaze_mapper.h"
#include "scorch/cloud/blaze/blaze_types.h"

using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelClient;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

class BlazeClient {
 public:
  BlazeClient();

  // Connects to the specified Blaze instance.
  ValueOf<void> Connect(const std::string& blaze_ip, int blaze_port,
                        const std::string& blaze_route,
                        const std::string& sentinel_ip, int sentinel_port,
                        const std::string& sentinel_route,
                        const std::string& project_path);

  ValueOf<void> SetMinecraftAccount(const std::string& minecraft_username);

  ValueOf<::nlohmann::json> Process(const std::string& command_name,
                                    const std::vector<::nlohmann::json>& args);

 private:
  typedef std::shared_ptr<ThriftHttpClient<::blaze::proto::BlazeClient>>
      ProtoBlazeClientPtr;

  ValueOf<::sentinel::proto::Token> LoadOrCreateToken();

  ProtoBlazeClientPtr http_client_ptr_;
  SentinelClient sentinel_client_;
  ::sentinel::proto::Token* token_;
  bool connected_;
  int blaze_port_;
  int sentinel_port_;
  std::string blaze_ip_;
  std::string blaze_route_;
  std::string project_path_;
  std::string sentinel_ip_;
  std::string sentinel_route_;
};

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BLAZE_BLAZE_CLIENT_H_
