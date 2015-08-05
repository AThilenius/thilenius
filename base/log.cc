// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/log.h"

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

Log::Log(LogLevel log_level) {
  switch (log_level) {
    case LogLevel::INFO: {
      stream_buffer_ << ConsoleColor::GREEN
                     << "[INFO]" << ConsoleColor::WHITE << ": ";
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
  }
}

Log::~Log() {
  stream_buffer_ << ConsoleColor::WHITE << std::endl;
  fprintf(stderr, "%s", stream_buffer_.str().c_str());
  fflush(stderr);
}

std::ostringstream& Log::GetOStringStream() { return stream_buffer_; }

}  // namespace base
}  // namespace thilenius
