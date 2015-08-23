// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>

#include "base/auto_update.h"
#include "base/curl/curl.h"
#include "base/dependency.h"
#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/log.h"
#include "base/path.h"
#include "base/process.h"
#include "base/string.h"

using ::thilenius::base::AutoUpdate;
using ::thilenius::base::Dependency;
using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::Process;
using ::thilenius::base::String;

int main(int argc, char** argv) {
  gflags::SetVersionString("0.0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  AutoUpdate::UpdateAtCurrentExecutable(
      "https://s3-us-west-1.amazonaws.com/thilenius.autoupdate/"
      "test_code_update.json");
  return 0;
}
