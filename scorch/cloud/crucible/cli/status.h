// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CLI_STATUS_H_
#define SCORCH_CLOUD_CRUCIBLE_CLI_STATUS_H_

#include <vector>
#include <string>

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace cli {

int Status (const std::string root_path, const std::vector<std::string>& args);

}  // namespace cli
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CLI_STATUS_H_