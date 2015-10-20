// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BLAZE_BLAZE_MODEL_H_
#define SCORCH_CLOUD_BLAZE_BLAZE_MODEL_H_

#include "base/value_of.hh"
#include "scorch/cloud/blaze/blaze_constants.h"
#include "scorch/cloud/blaze/blaze_mapper.h"
#include "scorch/cloud/blaze/blaze_types.h"
#include "third_party/mongo/client/dbclient.h"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

class BlazeModel {
 public:
  BlazeModel();

  ValueOf<::blaze::proto::MinecraftAccount> FindMinecraftAccountByUserUuid(
      const std::string& user_uuid);

  ValueOf<::blaze::proto::MinecraftAccount>
  FindMinecraftAccountByAccountUsername(const std::string& minecraft_username);

  bool SaveMinecraftAccount(const ::blaze::proto::MinecraftAccount& account);

 private:
  BlazeMapper blaze_mapper_;
  ::mongo::DBClientConnection connection_;
};

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BLAZE_BLAZE_MODEL_H_
