// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_
#define UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_

#include <vector>

#include "base/json.h"
#include "utils/differencer/differencer_constants.h"
#include "utils/differencer/differencer_types.h"

namespace thilenius {
namespace utils {
namespace differencer {

class DifferencerMapper {
 public:
  ::differencer::DiffType::type IntToDiffType(int value) const;

  int DiffTypeToInt(const ::differencer::DiffType::type& diff_type) const;

  ::differencer::Patch JsonToPatch(const ::nlohmann::json& json) const;

  ::nlohmann::json PatchToJson(const ::differencer::Patch& patch) const;
};

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius

#endif  // UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_
