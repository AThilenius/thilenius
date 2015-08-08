// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/anvil_snapshot.h"

namespace thilenius {
namespace scorch {
namespace anvil {

// static
AnvilSnapshot AnvilSnapshot::Generate(
    const std::vector<std::tuple<std::string, std::string>>& user_files,
    const std::vector<std::tuple<std::string, std::string>>& frozen_files) {
  AnvilSnapshot snapshot;
  // Snapshot user files
  return std::move(snapshot);
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
