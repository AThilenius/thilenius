// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_ARGUMENTS_H_
#define BASE_ARGUMENTS_H_

#include <string>
#include <vector>

namespace thilenius {
namespace base {

class Arguments {
 public:
  static std::vector<std::string> ToVector(int argc, char** argv);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_ARGUMENTS_H_
