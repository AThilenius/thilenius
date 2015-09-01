// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_CLI_LOGIN_H_
#define CLOUD_SENTINEL_CLI_LOGIN_H_

#include <vector>
#include <string>

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace cli {

int Login (const std::string root_path, const std::vector<std::string>& args);

}  // namespace cli
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_CLI_LOGIN_H_
