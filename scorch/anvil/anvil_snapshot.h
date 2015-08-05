// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_SNAPSHOT_H_
#define SCORCH_ANVIL_ANVIL_SNAPSHOT_H_

#include <string>

#include "base/macros.h"
#include "scorch/anvil/anvil_snapshot_constants.h"
#include "scorch/anvil/anvil_snapshot_types.h"

namespace thilenius {
namespace scorch {
namespace anvil {

class AnvilSnapshot {
  static AnvilSnapshot Load(const std::string &path);
}; // class AnvilSnapshot

} // namespace anvil
} // namespace scorch
} // namespace thilenius

#endif // SCORCH_ANVIL_ANVIL_SNAPSHOT_H_
