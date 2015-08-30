// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/regex.h"

#include <boost/regex.hpp>

namespace thilenius {
namespace base {

ValueOf<std::string> Regex::FindFirstMatch(const std::string& content,
                                           const std::string& regex) {
  std::vector<std::string> matches = Regex::FindAllMatches(content, regex);
  if (matches.size() == 0) {
    return {"", "Failed to find " + regex + " in " + content};
  }
  return {static_cast<std::string>(matches[0])};
}

std::vector<std::string> Regex::FindAllMatches(const std::string& content,
                                               const std::string& regex) {
  ::boost::regex regex_obj(regex);
  std::vector<std::string> matches;
  ::boost::smatch smatch;
  std::string::const_iterator start = content.begin();
  std::string::const_iterator end = content.end();
  while (boost::regex_search(start, end, smatch, regex_obj)) {
    std::string match_string(smatch[1].first, smatch[1].second);
    matches.push_back(match_string);
    // Update the beginning of the range to the character
    // following the whole match
    start = smatch[0].second;
  }
  return std::move(matches);
}

}  // namespace base
}  // namespace thilenius
