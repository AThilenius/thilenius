// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_INPUT_HH_
#define BASE_INPUT_HH_

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include "base/log.h"
#include "base/value_of.hh"

namespace thilenius {
namespace base {

class Input {
 public:
  template <typename T>
  static T WaitForever() {
    ValueOf<T> value = Read<T>();
    while (!value.IsValid()) {
      value = Read<T>();
    }
    return value.GetOrDie();
  }

  template <typename T>
  static T WaitOnceOrDie() {
    return Read<T>().GetOrDie();
  }

  template <typename T>
  static ValueOf<T> WaitOnce() {
    return Read<T>();
  }

 private:
  template <typename T>
  static ValueOf<T> Read() {
    std::cin.clear();
    std::string raw_input;
    T value;
    if (!std::getline(std::cin, raw_input)) {
      return {std::move(value), "Failed to read user input"};
    }
    std::stringstream ss(raw_input);
    if (!(ss >> value)) {
      return {std::move(value),
              "Failed to cast input to " + std::string(typeid(T).name())};
    }
    return {std::move(value)};
  }
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_INPUT_HH_
