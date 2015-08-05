// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_LOG_H_
#define BASE_LOG_H_

#include <sstream>

#include "base/macros.h"

namespace thilenius {
namespace base {

// Macros
#define LOG(level) \
  thilenius::base::Log(thilenius::base::LogLevel::level).GetOStringStream()

// Enums
enum class LogLevel { ERROR, WARNING, INFO };
enum class ConsoleColor { WHITE, RED, YELLOW, GREEN, BLUE };

// In-stream console colors
std::ostream& operator<<(std::ostream& stream, ConsoleColor color);

class Log {
 public:
  explicit Log(LogLevel log_level);
  ~Log();
  std::ostringstream& GetOStringStream();

 private:
  std::ostringstream stream_buffer_;
};  // class Log

}  // namespace base
}  // namespace thilenius

#endif  // BASE_LOG_H_
