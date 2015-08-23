// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_FILE_H_
#define BASE_FILE_H_

#include <string>
#include <memory>

#include "base/types.h"

namespace thilenius {
namespace base {

class File {
 public:
  static bool AppendToFile(const std::string& file, const std::string& content);
  static bool Exists(const std::string& file);
  static bool Remove(const std::string& file);
  static bool Rename(const std::string& file, const std::string& new_file);
  static bool WriteToFile(const std::string& file, const std::string& content);
  static int64 Size(const std::string& file);
  static std::string MD5OrDie(const std::string& file);
  static std::string ReadContentsOrDie(const std::string& path);
  static time_t LastWriteTime(const std::string& file);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_FILE_H_
