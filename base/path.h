// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_PATH_H_
#define BASE_PATH_H_

#include <ctime>
#include <string>

namespace thilenius {
namespace base {

class Path {
 public:
  static std::string Absolute(const std::string& relative_path);
  static std::string Combine(const std::string& left, const std::string& right);
  static std::string CurrentPath();
  static bool Exists(const std::string& path);
  static bool IsDirectory(const std::string& path);
  static bool IsRegularFile(const std::string& path);
  static std::time_t LastWriteTime(const std::string& path);
  static std::string ParentPath(const std::string& path);
  static std::string WithoutEdgeSlashes(const std::string& path);
  static std::string RelativePath(const std::string& full_path,
                                  const std::string head_path);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_PATH_H_
