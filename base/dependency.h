// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_DEPENDENCY_H_
#define BASE_DEPENDENCY_H_

#include <string>

namespace thilenius {
namespace base {

class Dependency {
 public:
  static std::string AptGetResolve(const std::string& name, bool silent);
  static int AptGetUpdate(bool silent);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_DEPENDENCY_H_
