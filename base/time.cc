// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/time.h"

#include <chrono>

#include "base/types.h"

namespace thilenius {
namespace base {

int64 Time::EpochMilliseconds() {
  return std::chrono::system_clock::now().time_since_epoch() /
      std::chrono::milliseconds(1);
}

}  // namespace base
}  // namespace thilenius
