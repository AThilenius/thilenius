// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_FIBER_FIBER_MAPPER_H_
#define CLOUD_FIBER_FIBER_MAPPER_H_

#include "cloud/fiber/fiber_constants.h"
#include "cloud/fiber/fiber_types.h"
#include "third_party/mongoxx/mongoxx.hh"

namespace thilenius {
namespace cloud {
namespace fiber {

class FiberMapper {
 public:
  FiberMapper();

  struct FiberEntry {
    ::fiber::proto::Cord cord;
    std::vector<::fiber::proto::Grain> grains;
  };

  ::mongoxx::Mapper<::fiber::proto::Cord> cord_mapper;
  ::mongoxx::Mapper<::fiber::proto::Grain> grain_mapper;
  ::mongoxx::Mapper<FiberEntry> fiber_entry_mapper;
};

}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_FIBER_FIBER_MAPPER_H_
