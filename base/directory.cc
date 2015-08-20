// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/directory.h"

#include <boost/filesystem.hpp>

#include "base/path.h"

namespace thilenius {
namespace base {

bool Directory::Create(const std::string& dir) {
  return ::boost::filesystem::create_directories(dir);
}

bool Directory::Exists(const std::string& dir) {
  return ::boost::filesystem::exists(dir) &&
         ::boost::filesystem::is_directory(dir);
}

time_t Directory::LastWriteTime(const std::string& dir) {
  return ::boost::filesystem::last_write_time(dir);
}

bool Directory::Remove(const std::string& dir) {
  return ::boost::filesystem::remove(dir);
}

int Directory::RemoveRecursive(const std::string& dir) {
  return static_cast<int>(::boost::filesystem::remove_all(dir));
}

bool Directory::Rename(const std::string& dir, const std::string& new_dir) {
  if (!Directory::Exists(dir)) {
    return false;
  } else {
    ::boost::filesystem::rename(dir, new_dir);
    return true;
  }
}

std::string Directory::TempDirectoryPath() {
  return std::move(::boost::filesystem::temp_directory_path().string());
}

std::vector<std::string> Directory::GetChildren(const std::string& dir) {
  ::boost::filesystem::directory_iterator end_iter;
  std::vector<std::string> sub_items;
  if (Directory::Exists(dir) && ::boost::filesystem::is_directory(dir)) {
    for (::boost::filesystem::directory_iterator dir_iter(dir);
         dir_iter != end_iter; dir_iter++) {
      sub_items.push_back(dir_iter->path().string());
    }
  }
  return std::move(sub_items);
}

std::vector<std::string> Directory::GetChildrenDirectories(
    const std::string& dir) {
  std::vector<std::string> sub_items = Directory::GetChildren(dir);
  std::vector<std::string> sub_dirs;
  for (const auto& sub_item : sub_items) {
    if (Path::IsDirectory(sub_item)) {
      sub_dirs.push_back(sub_item);
    }
  }
  return std::move(sub_dirs);
}

std::vector<std::string> Directory::GetChildrenFiles(const std::string& dir) {
  std::vector<std::string> sub_items = Directory::GetChildren(dir);
  std::vector<std::string> sub_files;
  for (const auto& sub_item : sub_items) {
    if (Path::IsRegularFile(sub_item)) {
      sub_files.push_back(sub_item);
    }
  }
  return std::move(sub_files);
}

std::vector<std::string> Directory::GetChildrenRecursive(
    const std::string& dir) {
  std::vector<std::string> sub_items = GetChildren(dir);
  for (const auto& item : sub_items) {
    if (Path::IsDirectory(item)) {
      std::vector<std::string> recursive_items = GetChildrenRecursive(item);
      sub_items.insert(sub_items.end(), recursive_items.begin(),
                       recursive_items.end());
    }
  }
  return std::move(sub_items);
}

std::vector<std::string> Directory::GetChildrenDirectoriesRecursive(
    const std::string& dir) {
  std::vector<std::string> sub_items = GetChildrenDirectories(dir);
  for (const auto& item : sub_items) {
    std::vector<std::string> recursive_items =
        GetChildrenDirectoriesRecursive(item);
    sub_items.insert(sub_items.end(), recursive_items.begin(),
                     recursive_items.end());
  }
  return std::move(sub_items);
}

std::vector<std::string> Directory::GetChildrenFilesRecursive(
    const std::string& dir) {
  std::vector<std::string> sub_items = GetChildrenFiles(dir);
  for (const auto& sub_dir : GetChildrenDirectories(dir)) {
    std::vector<std::string> recursive_items =
        GetChildrenFilesRecursive(sub_dir);
    sub_items.insert(sub_items.end(), recursive_items.begin(),
                     recursive_items.end());
  }
  return std::move(sub_items);
}

}  // namespace base
}  // namespace thilenius
