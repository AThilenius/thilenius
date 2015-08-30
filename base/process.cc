// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <boost/process.hpp>
#include <iostream>

#include "base/file.h"
#include "base/process.h"
#include "base/string.h"

namespace thilenius {
namespace base {

std::string Process::FindExecutable(const std::string& name) {
  if (File::Exists(name)) {
    return name;
  }
  std::string full_path;
  try {
    full_path = ::boost::process::find_executable_in_path(name);
  } catch (::boost::filesystem::filesystem_error& ex) {
  }
  return full_path;
}

int Process::ExecuteCommandSync(const std::string& command,
                                const std::vector<std::string> args,
                                bool silent) {
  std::function<void(std::string)> line_handler =
      silent ? [](std::string line) {}
             : [](std::string line) { std::cout << line << std::endl; };
  return ExecuteCommandSync(command, args, line_handler);
}

int Process::ExecuteCommandSync(
    const std::string& command, const std::vector<std::string> args,
    const std::function<void(std::string)> stdout_handler) {
  std::function<void(std::string)> stderr_handler =
      [&stdout_handler](std::string line) { stdout_handler(line); };
  return ExecuteCommandSync(command, args, stdout_handler, stderr_handler);
}

int Process::ExecuteCommandSync(
    const std::string& command, const std::vector<std::string> args,
    const std::function<void(std::string)> stdout_handler,
    const std::function<void(std::string)> stderr_handler) {
  std::string exec = FindExecutable(command);
  if (String::Blank(exec)) {
    return 1;
  }
  // Add command path to start of args
  std::vector<std::string> args_with_path = {exec};
  args_with_path.insert(args_with_path.end(), args.begin(), args.end());
  ::boost::process::context ctx;
  ctx.environment = ::boost::process::self::get_environment();
  ctx.stdout_behavior = ::boost::process::capture_stream();
  ctx.stderr_behavior = ::boost::process::capture_stream();
  ctx.stderr_behavior = ::boost::process::redirect_stream_to_stdout();
  ::boost::process::child child =
      ::boost::process::launch(exec, args_with_path, ctx);
  ::boost::process::pistream& stdout_stream = child.get_stdout();
  ::boost::process::pistream& stderr_stream = child.get_stderr();
  std::string stdout_line;
  std::string stderr_line;
  while (std::getline(stdout_stream, stdout_line) ||
         std::getline(stderr_stream, stderr_line)) {
    if (!String::Blank(stdout_line)) {
      stdout_handler(stdout_line);
      stdout_line = "";
    }
    if (!String::Blank(stderr_line)) {
      stderr_handler(stderr_line);
      stderr_line = "";
    }
  }
  ::boost::process::status status = child.wait();
  return (status.exited() ? !!status.exit_status() : EXIT_FAILURE);
}

}  // namespace base
}  // namespace thilenius
