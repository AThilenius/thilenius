// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef UTILS_DIFFERENCER_DIFFERENCER_H_
#define UTILS_DIFFERENCER_DIFFERENCER_H_

#include <string>
#include <vector>

#include "utils/differencer/differencer_constants.h"
#include "utils/differencer/differencer_types.h"

namespace thilenius {
namespace utils {
namespace differencer {

class Differencer {
 public:

  std::vector<::differencer::Patch> PatchesFromStrings(
      const std::string& from, const std::string& to) const;

  std::string ApplyPatches(
      const std::string& original,
      const std::vector<::differencer::Patch>& patches) const;

  void PrintPatchDebugString(const ::differencer::Patch& patch) const;
};

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius

#endif  // UTILS_DIFFERENCER_DIFFERENCER_H_
