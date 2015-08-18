// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_TIME_H_
#define BASE_TIME_H_

#include <string>

#include "base/types.h"

namespace thilenius {
namespace base {

class Time {
 public:
   static int64 EpochMilliseconds();
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_TIME_H_
