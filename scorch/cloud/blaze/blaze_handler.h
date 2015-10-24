// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BLAZE_BLAZE_HANDLER_H_
#define SCORCH_CLOUD_BLAZE_BLAZE_HANDLER_H_

#include <unordered_map>

#include "cloud/sentinel/sentinel_types.h"
#include "scorch/cloud/blaze/Blaze.h"
#include "scorch/cloud/blaze/blaze_types.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

class BlazeHandler : virtual public ::blaze::proto::BlazeIf {
 public:
  BlazeHandler();

  void ProcessBlazeRequest(std::string& _return,
                           const ::sentinel::proto::Token& token,
                           const std::string& command_name,
                           const std::vector<std::string>& args_json);

  void GetMinecraftAccount(::blaze::proto::MinecraftAccount& _return,
                           const ::sentinel::proto::Token& token);

  void SetMinecraftAccount(
      const ::sentinel::proto::Token& token,
      const ::blaze::proto::MinecraftAccount& minecraft_account);

 private:
  void ThrowOpFailure(const std::string& message);

  void AuthenticateOrThrow(const ::sentinel::proto::Token& token);
};

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BLAZE_BLAZE_HANDLER_H_
