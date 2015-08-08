// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_SNAPSHOT_H_
#define SCORCH_ANVIL_ANVIL_SNAPSHOT_H_

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "base/macros.h"
#include "scorch/anvil/anvil_snapshot_constants.h"
#include "scorch/anvil/anvil_snapshot_types.h"

namespace thilenius {
namespace scorch {
namespace anvil {

class AnvilSnapshot {
  public:
  static AnvilSnapshot Generate(
      const std::vector<std::tuple<std::string, std::string>>& user_files,
      const std::vector<std::tuple<std::string, std::string>>& frozen_files);

 private:
  std::unordered_map<std::string, std::string> user_file_contents_;
};  // class AnvilSnapshot

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_SNAPSHOT_H_
