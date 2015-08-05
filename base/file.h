// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_FILE_H_
#define BASE_FILE_H_

#include <string>
#include <memory>

namespace thilenius {
namespace base {

class File {
 public:
  static bool Exists(const std::string& path);
  static bool Delete(const std::string& path);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_FILE_H_
