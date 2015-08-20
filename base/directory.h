// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_DIRECTORY_H_
#define BASE_DIRECTORY_H_

#include <ctime>
#include <string>
#include <vector>

namespace thilenius {
namespace base {

class Directory {
 public:
  static bool Create(const std::string& dir);
  static bool Exists(const std::string& dir);
  static time_t LastWriteTime(const std::string& dir);
  static bool Remove(const std::string& dir);
  static int RemoveRecursive(const std::string& dir);
  static bool Rename(const std::string& dir, const std::string& new_dir);
  static std::string TempDirectoryPath();
  static std::vector<std::string> GetChildren(const std::string& dir);
  static std::vector<std::string> GetChildrenDirectories(
      const std::string& dir);
  static std::vector<std::string> GetChildrenFiles(const std::string& dir);
  static std::vector<std::string> GetChildrenRecursive(const std::string& dir);
  static std::vector<std::string> GetChildrenDirectoriesRecursive(
      const std::string& dir);
  static std::vector<std::string> GetChildrenFilesRecursive(
      const std::string& dir);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_DIRECTORY_H_
