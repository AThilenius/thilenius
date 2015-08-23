// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <boost/process.hpp>

#include "base/dependency.h"
#include "base/process.h"
#include "base/string.h"

namespace thilenius {
namespace base {

std::string Dependency::AptGetResolve(const std::string& name, bool silent) {
  std::string full_path = Process::FindExecutable(name);
  if (!String::Blank(full_path)) {
    return full_path;
  }
  if (Process::ExecuteCommandSync("apt-get", {"install", "-y", "-q", name},
                                  silent) != EXIT_SUCCESS) {
    // Try and update, then retry
    AptGetUpdate(silent);
    Process::ExecuteCommandSync("apt-get", {"install", "-y", "-q", name},
                                silent);
  }
  return Process::FindExecutable(name);
}

int Dependency::AptGetUpdate(bool silent) {
  return Process::ExecuteCommandSync("apt-get", {"update", "-y", "-q"}, silent);
}

}  // namespace base
}  // namespace thilenius
