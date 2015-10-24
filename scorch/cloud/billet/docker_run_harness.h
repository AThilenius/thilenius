// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BILLET_DOCKER_RUN_HARNESS_H_
#define SCORCH_CLOUD_BILLET_DOCKER_RUN_HARNESS_H_

#include <string>
#include <vector>

#include "base/process.h"
#include "base/value_of.hh"
#include "cloud/fiber/fiber_types.h"
#include "cloud/sentinel/sentinel_types.h"

using ::thilenius::base::ProcessPtr;
using ::thilenius::base::ValueOf;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

class DockerRunHarness {
 public:
  DockerRunHarness(const std::string& fiber_ip, int fiber_port,
                   const std::string& fiber_route);

  // Throws a ::billet::proto::SessionBusy directly
  ValueOf<::fiber::proto::Cord> HarnessRunAndDetatch(
      const sentinel::proto::Token& token, const std::string& mount_point,
      const std::string& shell_command);

 private:
  std::string fiber_ip_;
  int fiber_port_;
  std::string fiber_route_;
};

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BILLET_DOCKER_RUN_HARNESS_H_
