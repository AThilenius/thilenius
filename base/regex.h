// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_REGEX_H_
#define BASE_REGEX_H_

#include <string>
#include <vector>

#include "base/types.h"
#include "base/value_of.hh"

namespace thilenius {
namespace base {

class Regex {
 public:
  static ValueOf<std::string> FindFirstMatch(const std::string& content,
                                             const std::string& regex);
  static std::vector<std::string> FindAllMatches(const std::string& content,
                                                 const std::string& regex);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_REGEX_H_
