// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/arguments.h"

namespace thilenius {
namespace base {

std::vector<std::string> Arguments::ToVector(int argc, char** argv) {
  std::vector<std::string> output;
  for (int i = 0; i < argc; i++) {
    output.push_back(argv[i]);
  }
  return std::move(output);
}

}  // namespace base
}  // namespace thilenius
