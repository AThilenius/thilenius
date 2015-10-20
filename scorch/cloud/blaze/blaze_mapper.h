// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BLAZE_BLAZE_MAPPER_H_
#define SCORCH_CLOUD_BLAZE_BLAZE_MAPPER_H_

#include "scorch/cloud/blaze/blaze_constants.h"
#include "scorch/cloud/blaze/blaze_types.h"
#include "third_party/mongoxx/mongoxx.hh"
#include "utils/differencer/differencer_mapper.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

class BlazeMapper {
 public:
  BlazeMapper();

  ::mongoxx::Mapper<::blaze::proto::MinecraftAccount> minecraft_account_mapper;

};

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BLAZE_BLAZE_MAPPER_H_
