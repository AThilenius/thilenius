// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_HTTP_H_
#define BASE_HTTP_H_

#include <string>

#include "value_of.hh"

namespace thilenius {
namespace base {

class Http {
 public:
  static ValueOf<std::string> GetContent(const std::string& url);
  static std::string DownloadToFile(const std::string& url,
                                    const std::string& file_path);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_HTTP_H_
