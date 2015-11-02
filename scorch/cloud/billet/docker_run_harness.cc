// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/docker_run_harness.h"

#include <gflags/gflags.h>
#include <thread>

#include "base/guid.h"
#include "base/log.h"
#include "base/string.h"
#include "cloud/fiber/fiber_client.h"
#include "scorch/cloud/billet/billet_types.h"

using ::thilenius::base::Guid;
using ::thilenius::base::Process;
using ::thilenius::base::String;
using ::thilenius::cloud::fiber::FiberClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

DockerRunHarness::DockerRunHarness(const std::string& fiber_ip, int fiber_port,
                                   const std::string& fiber_route)
    : fiber_ip_(fiber_ip), fiber_port_(fiber_port), fiber_route_(fiber_route) {}

ValueOf<::fiber::proto::Cord> DockerRunHarness::HarnessRunAndDetatch(
    const sentinel::proto::Token& token, const std::string& mount_point,
    const std::string& shell_command) {
  // Check if alrleady running
  // docker inspect USER_GUID
  int exit_code =
      Process::ExecuteCommandSync("docker", {"inspect", token.user_uuid}, true);
  if (exit_code != EXIT_SUCCESS) {
    // Command already running
    throw ::billet::proto::SessionBusy();
  }
  // Create Cord
  FiberClient fiber_client;
  auto connection_value =
      fiber_client.Connect(fiber_ip_, fiber_port_, fiber_route_);
  if (!connection_value.IsValid()) {
    return {fiber::proto::Cord(), connection_value.GetError()};
  }
  auto cord_value = fiber_client.CreateCord(
      token, StrCat("billet-run-line-", Guid::NewGuid()));
  if (!cord_value.IsValid()) {
    return {fiber::proto::Cord(), cord_value.GetError()};
  }
  ::fiber::proto::Cord cord = cord_value.GetOrDie();
  // Execute:
  // docker run
  //   --detach
  //   --rm
  //   --tty
  //   --attach=STDOUT
  //   --attach=STDERR
  //   --name=USER_GUID
  //   --workdir="repo_path"
  //   --volume /billet/<USERID>:/billet/<USERID>
  //   athilenius/billet_client:prod
  //   /bin/bash -c 'while true; do echo hello; sleep 5; done'
  std::vector<std::string> full_args = {
      "run", "--detach", "--rm", "--tty", "--attach=STDOUT", "--attach=STDERR",
      StrCat("--name=", token.user_uuid), StrCat("--volume=", mount_point),
      "athilenius/docker_client:prod",
      StrCat("/bin/bash -c '", shell_command, "'")};
  // Create Thread (pass in token, command, args, cord)
  std::thread([token, full_args, fiber_client, cord]() mutable {
    // Don't care about the exit code. I think...?
    ProcessPtr docker_run = Process::FromExecv("docker", full_args);
    // Execute, don't block, and don't timeout
    docker_run->Execute(false, -1);
    // We are in non-blocking, so use this thread to pull everything from
    // cout/cerr and pipe it to Fiber
    for (int i = 0; true;) {
      auto ostream_value = docker_run->ReadOutputAfterIndex(i);
      if (!ostream_value.IsValid()) {
        return;
      }
      auto ostream_tokens = ostream_value.GetOrDie();
      // Convert them to grains
      std::vector<::fiber::proto::Grain> grains;
      for (const auto& ostream_token : ostream_tokens) {
        ::fiber::proto::Grain grain;
        grain.channel = ostream_token.is_err_stream ? 2 : 1;
        grain.data = ostream_token.content;
        grains.emplace_back(std::move(grain));
      }
      fiber_client.WriteCord(cord, grains);
      i += ostream_tokens.size();
    }
  }).detach();
  ::fiber::proto::Cord cord_copy = cord;
  return std::move(cord_copy);
}

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
