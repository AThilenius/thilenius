// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/file.h"

#include <boost/filesystem.hpp>
#include <fstream>

#include "base/log.h"

namespace thilenius {
namespace base {

bool File::AppendToFile(const std::string& file, const std::string& content) {
  std::ofstream file_stream(file, std::ios_base::app);
  if (!file_stream) {
    return false;
  }
  return (file_stream << content);
}

bool File::Exists(const std::string& file) {
  return ::boost::filesystem::exists(file) &&
         ::boost::filesystem::is_regular_file(file);
}

time_t File::LastWriteTime(const std::string& file) {
  return ::boost::filesystem::last_write_time(file);
}

std::string File::ReadContentsOrDie(const std::string& path) {
  if (!File::Exists(path)) {
    LOG(FATAL) << "Failed to find file: " << path;
  }
  std::ifstream file_stream(path);
  if (!file_stream) {
    LOG(FATAL) << "Failed to open file: " << path;
  }
  std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
                            std::istreambuf_iterator<char>());
  return std::move(file_contents);
}

bool File::Remove(const std::string& file) {
  if (!File::Exists(file)) {
    return false;
  }
  return ::boost::filesystem::remove(file);
}

bool File::Rename(const std::string& file, const std::string& new_file) {
  if (!File::Exists(file)) {
    return false;
  }
  ::boost::filesystem::rename(file, new_file);
  return true;
}

bool File::WriteToFile(const std::string& file, const std::string& content) {
  if (File::Exists(file) && !File::Remove(file)) {
    return false;
  }
  return File::AppendToFile(file, content);
}

}  // namespace base
}  // namespace thilenius
