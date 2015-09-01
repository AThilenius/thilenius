// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/log.h"

#include <iostream>
#ifdef __linux__
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#endif

namespace thilenius {
namespace base {

namespace {

#ifdef __linux__
// This is taken from: http://panthema.net/2008/0901-stacktrace-demangled/
/** Print a demangled stack backtrace of the caller function to FILE* out. */
void PrintStackTrace(FILE* out = stderr, unsigned int max_frames = 63) {
  fprintf(out, "[FATAL]: Stack trace:\n");
  // storage array for stack trace address data
  void* addrlist[max_frames + 1];
  // retrieve current stack addresses
  int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
  if (addrlen == 0) {
    fprintf(out, "  <empty, possibly corrupt>\n");
    return;
  }
  // resolve addresses into strings containing "filename(function+address)",
  // this array must be free()-ed
  char** symbollist = backtrace_symbols(addrlist, addrlen);
  // allocate string which will be filled with the demangled function name
  size_t funcnamesize = 256;
  char* funcname = (char*)malloc(funcnamesize);
  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for (int i = 2; i < addrlen; i++) {
    char* begin_name = 0, * begin_offset = 0, * end_offset = 0;
    // find parentheses and +address offset surrounding the mangled name:
    // ./module(function+0x15c) [0x8048a6d]
    for (char* p = symbollist[i]; *p; ++p) {
      if (*p == '(')
        begin_name = p;
      else if (*p == '+')
        begin_offset = p;
      else if (*p == ')' && begin_offset) {
        end_offset = p;
        break;
      }
    }
    if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';
      *end_offset = '\0';
      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():
      int status;
      char* ret =
          abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
      if (status == 0) {
        funcname = ret;  // use possibly realloc()-ed string
        fprintf(out, "[FATAL]:  %s: %s\n", symbollist[i], funcname);
      } else {
        // demangling failed. Output function name as a C function with
        // no arguments.
        fprintf(out, "[FATAL]:  %s: %s()\n", symbollist[i], begin_name);
      }
    } else {
      // couldn't parse the line? print the whole line.
      fprintf(out, "[FATAL]:  %s\n", symbollist[i]);
    }
  }
  free(funcname);
  free(symbollist);
}
#endif  // __linux__

}  // namespace

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

Log::Log(LogLevel log_level) : log_level_(log_level) {
  switch (log_level) {
    case LogLevel::INPUT: {
      stream_buffer_ << ConsoleColor::GREEN << "[INPUT]" << ConsoleColor::WHITE
                     << ": ";
      break;
    }
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
      stream_buffer_ << ConsoleColor::RED << "[FATAL]: ";
      break;
    }
  }
}

Log::~Log() {
  // Skip last \n if INPUT
  if (log_level_ == LogLevel::INPUT) {
    stream_buffer_ << ConsoleColor::WHITE;
  } else {
    stream_buffer_ << ConsoleColor::WHITE << std::endl;
  }
  if (log_level_ == LogLevel::FATAL) {
// Run a stack trace if on linux
#ifdef __linux__
    PrintStackTrace();
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
