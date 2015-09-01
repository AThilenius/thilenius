// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/cli/login.h"

#include "base/input.hh"
#include "base/log.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_user.h"

using ::thilenius::base::Input;
using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelUser;

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace cli {

int Login(const std::string root_path, const std::vector<std::string>& args) {
  LOG(INPUT) << "Email address: ";
  std::string email_address = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Password: ";
  std::string password = Input::WaitOnceOrDie<std::string>();
  SentinelUser sentinel_user =
      SentinelUser::Login(email_address, password).GetOrDie();
  LOG(INFO) << "Login successful";
  return 0;
}

}  // namespace cli
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
