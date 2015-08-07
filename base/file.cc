// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/file.h"

#include <cstdio>
#include <fstream>
#include <fstream>
#include <streambuf>
#include <string>

#include "base/log.h"

namespace thilenius {
namespace base {

bool File::Exists(const std::string& path) {
  return std::ifstream(path.c_str()) != 0;
}

bool File::Delete(const std::string& path) { return remove(path.c_str()) == 0; }

std::string File::ReadContentsOrDie(const std::string& path) {
  if (!File::Exists(path)) {
    LOG(FATAL) << "Failed to find file: " << path;
  }
  std::ifstream file_stream(path);
  if(!file_stream) {
    LOG(FATAL) << "Failed to open file: " << path;
  }
  std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
                            std::istreambuf_iterator<char>());
  return std::move(file_contents);
}

}  // namespace base
}  // namespace thilenius
