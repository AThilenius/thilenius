// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/file.h"

#include <cstdio>
#include <fstream>

namespace thilenius {
namespace base {

bool File::Exists(const std::string& path) { return std::ifstream(path) != 0; }

bool File::Delete(const std::string& path) { return remove(path.c_str()) == 0; }

}  // namespace base
}  // namespace thilenius
