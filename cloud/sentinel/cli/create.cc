// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/cli/create.h"

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

int Create(const std::string root_path, const std::vector<std::string>& args) {
  ::sentinel::User user;
  LOG(INPUT) << "First name: ";
  user.first_name = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Last name: ";
  user.last_name = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Email address: ";
  user.email_address = Input::WaitOnceOrDie<std::string>();
  LOG(INPUT) << "Password: ";
  std::string password = Input::WaitOnceOrDie<std::string>();

  SentinelUser sentinel_user = SentinelUser::Create(user, password).GetOrDie();
  LOG(INFO) << "Created a new user, with primary token: "
            << sentinel_user.PrimaryToken();
  LOG(INFO) << "Token: "
            << sentinel_user.PrimaryToken().token_uuid;
  return 0;
}

}  // namespace cli
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
