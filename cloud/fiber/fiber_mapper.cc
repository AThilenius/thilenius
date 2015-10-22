// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/fiber/fiber_mapper.h"

namespace thilenius {
namespace cloud {
namespace fiber {

FiberMapper::FiberMapper() {
  // Cord
  cord_mapper.add_field("uuid", &::fiber::proto::Cord::uuid);
  cord_mapper.add_field("name", &::fiber::proto::Cord::name);
  cord_mapper.add_field("user_uuid", &::fiber::proto::Cord::user_uuid);
  cord_mapper.add_field("timestamp", &::fiber::proto::Cord::timestamp);

  // Grain (Shortened names as there will be millions of these)
  grain_mapper.add_field("c", &::fiber::proto::Grain::channel);
  grain_mapper.add_field("d", &::fiber::proto::Grain::data);

  // FiberEntry
  fiber_entry_mapper.add_field("cord", &FiberMapper::FiberEntry::cord,
                               cord_mapper);
  fiber_entry_mapper.add_field("grains", &FiberMapper::FiberEntry::grains,
                               grain_mapper);
}

}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius
