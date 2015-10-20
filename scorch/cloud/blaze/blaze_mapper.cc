// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/blaze/blaze_mapper.h"

#include "base/log.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

BlazeMapper::BlazeMapper() {
  minecraft_account_mapper.add_field(
      "user_uuid", &::blaze::proto::MinecraftAccount::user_uuid);
  minecraft_account_mapper.add_field(
      "minecraft_username",
      &::blaze::proto::MinecraftAccount::minecraft_username);
}

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
