
// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/path.h"

#include <unistd.h>
#include <limits.h>

namespace thilenius {
namespace base {

std::string Path::ExecutablePath() {
#ifdef __linux__
  char buff[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
  if (len != -1) {
    buff[len] = '\0';
    return std::string(buff);
  }
  return "";
#else
  #error Unknown OS type
#endif
}

} // namespace base
} // namespace thilenius
