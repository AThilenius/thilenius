// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_GUID_H_
#define BASE_GUID_H_

#include <string>

namespace thilenius {
namespace base {

class Guid {
 public:
  static std::string NewGuid();
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_GUID_H_
