// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/billet_session.h"

#include <gflags/gflags.h>
#include <thread>

#include "base/directory.h"
#include "base/file.h"
#include "base/guid.h"
#include "base/log.h"
#include "base/path.h"
#include "base/string.h"
#include "cloud/fiber/fiber_client.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/Crucible.h"

DEFINE_string(host_billet_mount_point, "",
              "The route (ON THE HOST) to the billet folder");

using ::thilenius::base::AsyncProcess;
using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Guid;
using ::thilenius::base::Path;
using ::thilenius::base::String;
using ::thilenius::cloud::fiber::FiberClient;
using ::thilenius::cloud::fiber::FiberClientPtr;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

BilletSessionPtr BilletSession::CreateSession(const std::string& fiber_ip,
                                              int fiber_port,
                                              const std::string& fiber_route) {
  BilletSessionPtr session_ptr(new BilletSession());
  session_ptr->fiber_ip_ = fiber_ip;
  session_ptr->fiber_port_ = fiber_port;
  session_ptr->fiber_route_ = fiber_route;
  session_ptr->process_ = nullptr;
  return session_ptr;
}

ValueOf<::fiber::proto::Cord> BilletSession::HarnessRunAndDetatch(
    const sentinel::proto::Token& token, const std::string& mount_point,
    const std::string& shell_command) {
  if (FLAGS_host_billet_mount_point == "") {
    LOG(ERROR) << "FORGOT TO SET --host_billet_mount_point";
    return {::fiber::proto::Cord(),
            "Woops, Billet wasn't set up correctly. Please tell Alec"};
  }
  std::unique_lock<std::mutex> lock(mutex_);
  if (process_ != nullptr && !process_->DidExit()) {
    throw ::billet::proto::SessionBusy();
  }
  // Create Cord
  FiberClientPtr fiber_client_ptr(new FiberClient());
  auto connection_value =
      fiber_client_ptr->Connect(fiber_ip_, fiber_port_, fiber_route_);
  if (!connection_value.IsValid()) {
    return {fiber::proto::Cord(), connection_value.GetError()};
  }
  auto cord_value = fiber_client_ptr->CreateCord(
      token, StrCat("billet-run-line-", Guid::NewGuid()));
  if (!cord_value.IsValid()) {
    return {fiber::proto::Cord(), cord_value.GetError()};
  }
  ::fiber::proto::Cord cord = cord_value.GetOrDie();
  // Save it for later use as well
  cord_ = cord;
  // Write the command out to a shell scrtip
  // TODO(athilenius): To to figure out how to pass it all as args
  std::string shell_scropt_path = Path::Combine(mount_point, "billet_run.sh");
  std::string shell_script = StrCat(
      // Fail on first error
      "set -e\n",
      // CD to the mount point
      "cd ", mount_point, "\n",
      // The C Compiler to use
      "export CC=/usr/bin/clang-3.6\n",
      // The C++ Compiler to user
      "export CXX=/usr/bin/clang++-3.6\n",
      // Run the shell command given to us
      shell_command, "\n");
  if (!File::WriteToFile(shell_scropt_path, shell_script)) {
    return {::fiber::proto::Cord(),
            "Failed to write shell script for container execution."};
  }
  std::vector<std::string> full_command = {
      //  Docker Run
      "run",
      // Remove when done
      "--rm",
      // Pretend to be TTY
      //"--tty",
      // Name the container User ID
      "--name", token.user_uuid,
      // Mount $HOME/billet/USERID
      "--volume",
      StrCat(Path::Combine(FLAGS_host_billet_mount_point, token.user_uuid),
             ":/billet/", token.user_uuid),
      // The image to run
      "athilenius/billet_client:prod",
      // Command to execute
      "/bin/bash", shell_scropt_path};
  auto output_handler =
      [fiber_client_ptr, cord](int channel, std::string content) {
        ::fiber::proto::Grain grain;
        grain.channel = channel;
        grain.data = content;
        fiber_client_ptr->WriteCord(cord, {grain});
      };
  auto end_handler = [this, fiber_client_ptr, cord](int exit_code) {
    std::unique_lock<std::mutex> lock(mutex_);
    ::fiber::proto::Grain grain;
    grain.channel = 1;
    grain.data =
        StrCat("Process exited with code: ", std::to_string(exit_code));
    fiber_client_ptr->WriteCord(cord, {grain});
    fiber_client_ptr->CloseCord(cord);
    cond_var_.notify_all();
  };
  process_ = AsyncProcess::FromExecv("/bin/docker", full_command,
                                     output_handler, end_handler);
  ::fiber::proto::Cord cord_copy = cord;
  return std::move(cord_copy);
}

void BilletSession::TerminateSession(const sentinel::proto::Token& token) {
  {
    // Don't duplicate terminate processes
    std::unique_lock<std::mutex> lock(mutex_);
    if (terminate_process_ && !terminate_process_->DidExit()) {
      return;
    }
  }
  // rm -rf the docker image, ignore output
  std::vector<std::string> rm_command = {"rm", "-f", token.user_uuid};
  // Ignore it
  auto output_handler = [](int channel, std::string content) {};
  // Ignore it
  auto end_handler = [](int exit_code) {};
  terminate_process_ = AsyncProcess::FromExecv("/bin/docker", rm_command,
                                               output_handler, end_handler);
}

::billet::proto::SessionStatus BilletSession::GetSessionStatus() {
  std::unique_lock<std::mutex> lock(mutex_);
  ::billet::proto::SessionStatus session_status;
  session_status.is_running = process_ != nullptr && !process_->DidExit();
  session_status.current_or_last_cord = cord_;
  return std::move(session_status);
}

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
