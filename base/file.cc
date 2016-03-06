// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/file.h"

#include <boost/filesystem.hpp>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <openssl/md5.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "base/log.h"

namespace thilenius {
namespace base {

// Static
ValueOf<std::string> File::ReadContents(const std::string& path) {
  if (!File::Exists(path)) {
    return {"", StrCat("Failed to find file: ", path)};
  }
  std::ifstream file_stream(path);
  if (!file_stream) {
    return {"", StrCat("Failed to open file: ", path)};
  }
  std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
                            std::istreambuf_iterator<char>());
  return {std::move(file_contents)};
}

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

std::string File::MD5OrDie(const std::string& file) {
  if (!File::Exists(file)) {
    LOG(FATAL) << "Failed to find file: " << file;
  }
  uint8 result[MD5_DIGEST_LENGTH];
  int file_descript = open(file.c_str(), O_RDONLY);
  if (!file_descript) {
    LOG(FATAL) << "Failed to find file: " << file;
  }
  int64 file_size = File::Size(file);
  char* file_buffer = static_cast<char*>(
      mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0));
  ::MD5((unsigned char*)file_buffer, file_size, result);
  munmap(file_buffer, file_size);
  std::ostringstream sout;
  sout << std::hex << std::setfill('0');
  for (const auto& c : result) {
    sout << std::setw(2) << (int)c;
  }
  return sout.str();
}

std::string File::ReadContentsOrDie(const std::string& path) {
  return File::ReadContents(path).GetOrDie();
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

int64 File::Size(const std::string& file) {
  return static_cast<int64>(::boost::filesystem::file_size(file));
}

bool File::WriteToFile(const std::string& file, const std::string& content) {
  if (File::Exists(file) && !File::Remove(file)) {
    return false;
  }
  return File::AppendToFile(file, content);
}

}  // namespace base
}  // namespace thilenius
