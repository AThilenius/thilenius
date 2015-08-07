
// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/path.h"

#include <limits.h>
#include <unistd.h>

#include "base/string.h"

namespace thilenius {
namespace base {

std::string Path::ExecutablePath() {
#ifdef __linux__
  char buff[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
  if (len != -1) {
    buff[len] = '\0';
    return std::move(std::string(buff));
  }
  return "";
#else
#error Unknown OS type
#endif
}

std::string Path::Combine(const std::string& left, const std::string& right) {
  if (EndsWith(left, "/")) {
    if (BeginsWith(right, "/")) {
      return StrCat(left.substr(0, left.length() - 1), "/",
                    right.substr(1, right.length() - 1));
    } else {
      return StrCat(left.substr(0, left.length() - 1), "/", right);
    }
  } else {
    if (BeginsWith(right, "/")) {
      return StrCat(left, "/", right.substr(1, right.length() - 1));
    } else {
      return StrCat(left, "/", right);
    }
  }
}

std::string Path::RelativeToAbsolutePath(const std::string& path) {
  // See if the file exists with the exe path prepended
  std::string full_path = Path::Combine(ExecutablePath(), path);
}

}  // namespace base
}  // namespace thilenius
