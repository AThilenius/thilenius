// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_VALUE_OF_H_
#define BASE_VALUE_OF_H_

#include <string>

#include "base/log.h"

namespace thilenius {
namespace base {

template <typename T>
class ValueOf {
 public:
  ValueOf(T&& value) : value_(value), is_valid_(true) {}
  ValueOf(T&& value, std::string error)
      : value_(value), error_(std::move(error)), is_valid_(false) {}

  T GetOrDie() {
    if (!IsValid()) {
      LOG(FATAL) << error_;
    }
    is_valid_ = false;
    return std::move(value_);
  }

  bool IsValid() { return is_valid_; }

  std::string GetError() {
    return error_;
  }

 private:
  T value_;
  std::string error_;
  bool is_valid_;
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_VALUE_OF_H_
