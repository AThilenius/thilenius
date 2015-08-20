// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/path.h"

#include <boost/filesystem.hpp>

#include "string.h"

namespace thilenius {
namespace base {

std::string Path::Absolute(const std::string& relative_path) {
  return ::boost::filesystem::absolute(relative_path).string();
}

std::string Path::Combine(const std::string& left, const std::string& right) {
  std::string left_side = left;
  if (EndsWith(left_side, "/") && BeginsWith(right, "/")) {
    left_side = left_side.substr(0, left_side.length() - 1);
  }
  return (::boost::filesystem::path(left_side) /
          ::boost::filesystem::path(right)).string();
}

std::string Path::CurrentPath() {
  return ::boost::filesystem::current_path().string();
}

bool Path::Exists(const std::string& path) {
  return ::boost::filesystem::exists(path);
}

bool Path::IsDirectory(const std::string& path) {
  return ::boost::filesystem::is_directory(path);
}

bool Path::IsRegularFile(const std::string& path) {
  return ::boost::filesystem::is_regular_file(path);
}

std::time_t Path::LastWriteTime(const std::string& path) {
  return ::boost::filesystem::last_write_time(path);
}

std::string Path::ParentPath(const std::string& path) {
  boost::filesystem::path p(path);
  return p.parent_path().string();
}

std::string Path::WithoutEdgeSlashes(const std::string& path) {
  std::string path_p = path;
  while (String::BeginsWith(path_p, "/")) {
    path_p = String::RemoveFromBeginning(path_p, "/");
  }
  while (String::EndsWith(path_p, "/")) {
    path_p = String::RemoveFromEnd(path_p, "/");
  }
  return std::move(path_p);
}

std::string Path::RelativePath(const std::string& full_path,
                               const std::string head_path) {
  std::string head = Path::WithoutEdgeSlashes(head_path);
  std::string full = Path::WithoutEdgeSlashes(full_path);
  return std::move(
      Path::WithoutEdgeSlashes(String::RemoveFromBeginning(full, head)));
}

}  // namespace base
}  // namespace thilenius
