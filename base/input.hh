// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_INPUT_HH_
#define BASE_INPUT_HH_

#include <iostream>
#include <sstream>
#include <string>
#include <termios.h>
#include <typeinfo>
#include <unistd.h>

#include "base/log.h"
#include "base/value_of.hh"

namespace thilenius {
namespace base {

class Input {
 public:
  template <typename T>
  static T WaitForever() {
    return WaitForever<T>(false);
  }

  template <typename T>
  static T WaitOnceOrDie() {
    return WaitOnceOrDie<T>(false);
  }

  template <typename T>
  static ValueOf<T> WaitOnce() {
    return WaitOnce<T>(false);
  }

  template <typename T>
  static T WaitForever(bool suppress_cout) {
    SetStdinEcho(!suppress_cout);
    ValueOf<T> value = Read<T>();
    while (!value.IsValid()) {
      value = Read<T>();
    }
    SetStdinEcho(true);
    return value.GetOrDie();
  }

  template <typename T>
  static T WaitOnceOrDie(bool suppress_cout) {
    SetStdinEcho(!suppress_cout);
    ValueOf<T> val = Read<T>();
    SetStdinEcho(true);
    return val.GetOrDie();
  }

  template <typename T>
  static ValueOf<T> WaitOnce(bool suppress_cout) {
    SetStdinEcho(!suppress_cout);
    ValueOf<T> val = Read<T>();
    SetStdinEcho(true);
    return std::move(val);
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

  static void SetStdinEcho(bool enable) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
      tty.c_lflag &= ~ECHO;
    else
      tty.c_lflag |= ECHO;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
  }
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_INPUT_HH_
