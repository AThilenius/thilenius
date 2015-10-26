// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/billet_session.h"

#include <gflags/gflags.h>

#include "base/guid.h"
#include "base/log.h"
#include "base/path.h"
#include "base/process.h"
#include "base/string.h"
#include "cloud/fiber/fiber_client.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/Crucible.h"

using ::thilenius::base::Guid;
using ::thilenius::base::Path;
using ::thilenius::base::Process;
using ::thilenius::base::String;
using ::thilenius::cloud::fiber::FiberClient;
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
  std::unique_lock<std::mutex> lock(mutex_);
  if (process_ != nullptr) {
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
  // Save it for later use as well
  cord_ = cord;
  std::string full_bash_command =
      StrCat("CC=/usr/bin/clang-3.6;", "CXX=/usr/bin/clang++-3.6;", "cd ",
             mount_point, ";", shell_command);
  process_ = Process::FromExecv("/bin/bash", {"-c", full_bash_command});
  // Execute, don't block, and don't timeout
  process_->Execute(false, -1);
  // Create a new thread to read cout/cerr and write it to Fiber
  std::thread([this, token, fiber_client, cord]() mutable {
    for (int i = 0; true;) {
      auto ostream_value = process_->ReadOutputAfterIndex(i);
      if (!ostream_value.IsValid()) {
        break;
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
    {
      std::unique_lock<std::mutex> lock(mutex_);
      // Process done
      ::fiber::proto::Grain grain;
      grain.channel = 1;
      grain.data = StrCat("Process exited with code: ",
                          std::to_string(process_->GetExitCode()));
      fiber_client.WriteCord(cord, {grain});
      fiber_client.CloseCord(cord);
      process_ = nullptr;
      cond_var_.notify_all();
    }
  }).detach();
  ::fiber::proto::Cord cord_copy = cord;
  return std::move(cord_copy);
}

void BilletSession::TerminateSession() {
  std::unique_lock<std::mutex> lock(mutex_);
  if (process_ == nullptr) {
    return;
  }
  process_->Kill(true);
  // Wait for Fiber thread to close
  while (process_ != nullptr) {
    cond_var_.wait(lock);
  }
}

::billet::proto::SessionStatus BilletSession::GetSessionStatus() {
  std::unique_lock<std::mutex> lock(mutex_);
  ::billet::proto::SessionStatus session_status;
  session_status.is_running = process_ != nullptr;
  session_status.current_or_last_cord = cord_;
  return std::move(session_status);
}

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
