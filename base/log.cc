// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/log.h"

#include <iostream>
#ifdef __linux__
#include <execinfo.h>
#endif

namespace thilenius {
namespace base {

std::ostream& operator<<(std::ostream& stream, ConsoleColor color) {
#if !defined(DISABLE_COLORS)
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#error Colors not implemented on windows, stream is flushed atomicly
#else
  switch (color) {
    case ConsoleColor::WHITE: {
      stream << "\033[0m";
      break;
    }
    case ConsoleColor::RED: {
      stream << "\033[31m";
      break;
    }
    case ConsoleColor::YELLOW: {
      stream << "\033[33m";
      break;
    }
    case ConsoleColor::GREEN: {
      stream << "\033[32m";
      break;
    }
    case ConsoleColor::BLUE: {
      stream << "\033[36m";
      break;
    }
  }
#endif
#endif
  return stream;
}

Log::Log(LogLevel log_level) : is_fatal_(false) {
  switch (log_level) {
    case LogLevel::INFO: {
      stream_buffer_ << ConsoleColor::GREEN << "[INFO]" << ConsoleColor::WHITE
                     << ": ";
      break;
    }
    case LogLevel::WARNING: {
      stream_buffer_ << ConsoleColor::YELLOW << "[WARN]: ";
      break;
    }
    case LogLevel::ERROR: {
      stream_buffer_ << ConsoleColor::RED << "[EROR]: ";
      break;
    }
    case LogLevel::FATAL: {
      // Set the fatal flag for stack tracing and exiting
      is_fatal_ = true;
      std::cout << ConsoleColor::RED
                << "A fatal ::thilenius::base::log  was raised." << std::endl;
      std::cout.flush();
      stream_buffer_ << ConsoleColor::RED << "[FATL]: ";
      break;
    }
  }
}

Log::~Log() {
  stream_buffer_ << ConsoleColor::WHITE << std::endl;
  if (is_fatal_) {
    // Run a stack trace if on linux
#ifdef __linux__
    void* buffer[10];
    size_t number_pointers = backtrace(buffer, 10);
    char** strings = backtrace_symbols(buffer, number_pointers);
    if (strings != nullptr) {
      for (int i = 0; i < number_pointers; i++) {
        std::cout << ConsoleColor::WHITE << strings[i] << std::endl;
      }
    }
    free(strings);
#endif
    fprintf(stderr, "%s", stream_buffer_.str().c_str());
    fflush(stderr);
    exit(EXIT_FAILURE);
  } else {
    fprintf(stderr, "%s", stream_buffer_.str().c_str());
    fflush(stderr);
  }
}

std::ostringstream& Log::GetOStringStream() { return stream_buffer_; }

}  // namespace base
}  // namespace thilenius
