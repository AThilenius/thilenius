// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>

#include "base/file.h"
#include "base/log.h"
#include "base/path.h"
#include "base/process.h"
#include "base/string.h"

using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::Process;
using ::thilenius::base::ProcessPtr;
using ::thilenius::base::String;

int main(int argc, char** argv) {
  std::string bash_command = "while true; do echo \"hello\"; sleep 1; done;";
  ProcessPtr process = Process::FromExecv("/bin/bash", {"-c", bash_command});
  process->Execute(false, 10000);

  int last_index = 0;
  while (true) {
    auto output = process->ReadOutputAfterIndex(last_index).GetOrDie();
    std::cout << "Got:";
    for (const auto& item : output) {
      std::cout << (item.is_err_stream ? "Error: " : "") << item.content;
    }
    last_index += output.size();
  }

  std::cin.ignore();
  return 0;
}
