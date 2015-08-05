// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_PATH_H_
#define BASE_PATH_H_

#include <string>

namespace thilenius {
namespace base {

class Path {
 public:
   static std::string ExecutablePath();
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_PATH_H_
