// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/string.h"

namespace thilenius {
namespace base {

bool Empty(const std::string& str) { return str.length() == 0; }

bool Blank(const std::string& str) {
  if (str.length() == 0) {
    return true;
  }
  for (const auto& chr : str) {
    if (chr != ' ') {
      return false;
    }
  }
  return true;
}

bool BeginsWith(const std::string& str, const std::string& other_str) {
  return str.compare(0, other_str.length(), other_str) == 0;
}

bool EndsWith(const std::string& str, const std::string& other_str) {
  if (str.length() >= other_str.length()) {
    return str.compare(str.length() - other_str.length(), other_str.length(),
                       other_str) == 0;
  } else {
    return false;
  }
}

}  // namespace base
}  // namespace thilenius
