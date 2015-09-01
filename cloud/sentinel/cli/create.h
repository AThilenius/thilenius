// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_CLI_CREATE_H_
#define CLOUD_SENTINEL_CLI_CREATE_H_

#include <vector>
#include <string>

namespace thilenius {
namespace cloud {
namespace sentinel {
namespace cli {

int Create (const std::string root_path, const std::vector<std::string>& args);

}  // namespace cli
}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_CLI_CREATE_H_
