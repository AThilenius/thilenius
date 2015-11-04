// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/string.h"

#include <algorithm>

using ::thilenius::base::String;

bool Empty(const std::string& str) { return String::Empty(str); }

bool Blank(const std::string& str) { return String::Blank(str); }

bool BeginsWith(const std::string& str, const std::string& other_str) {
  return String::BeginsWith(str, other_str);
}

bool EndsWith(const std::string& str, const std::string& other_str) {
  return String::EndsWith(str, other_str);
}

namespace thilenius {
namespace base {

bool String::BeginsWith(const std::string& str, const std::string& other_str) {
  return str.compare(0, other_str.length(), other_str) == 0;
}

bool String::Blank(const std::string& str) {
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

bool String::Empty(const std::string& str) { return str.length() == 0; }

bool String::EndsWith(const std::string& str, const std::string& other_str) {
  if (str.length() >= other_str.length()) {
    return str.compare(str.length() - other_str.length(), other_str.length(),
                       other_str) == 0;
  } else {
    return false;
  }
}

std::string String::RemoveFromBeginning(const std::string& from_string,
                                        const std::string& value) {
  if (!String::BeginsWith(from_string, value) || String::Blank(value) ||
      String::Blank(from_string)) {
    return from_string;
  }
  return from_string.substr(value.length(),
                            from_string.length() - value.length());
}

std::string String::RemoveFromEnd(const std::string& from_string,
                                  const std::string& value) {
  if (!String::EndsWith(from_string, value) || String::Blank(value) ||
      String::Blank(from_string)) {
    return from_string;
  }
  return from_string.substr(0, from_string.length() - value.length());
}

std::string String::ToLower(const std::string& str) {
  std::string data = str;
  std::transform(data.begin(), data.end(), data.begin(), ::tolower);
  return std::move(data);
}

std::string String::ToUpper(const std::string& str) {
  std::string data = str;
  std::transform(data.begin(), data.end(), data.begin(), ::toupper);
  return std::move(data);
}

} // namespace base
} // namespace thilenius
