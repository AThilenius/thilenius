// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_SENTINEL_MAPPER_H_
#define CLOUD_SENTINEL_SENTINEL_MAPPER_H_

#include "cloud/sentinel/gen-cpp2/sentinel_constants.h"
#include "cloud/sentinel/gen-cpp2/sentinel_types.h"
#include "third_party/mongoxx/mongoxx.hh"

namespace thilenius {
namespace cloud {
namespace sentinel {

class SentinelMapper {
 public:
  SentinelMapper();

  struct SentinelEntry {
    ::sentinel::proto::User user;
    std::string salt;
    std::string hash;
  };

  ::mongoxx::Mapper<::sentinel::proto::User> user_mapper;
  ::mongoxx::Mapper<::sentinel::proto::Token> token_mapper;
  ::mongoxx::Mapper<SentinelEntry> entry_mapper;
};

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_SENTINEL_MAPPER_H_
