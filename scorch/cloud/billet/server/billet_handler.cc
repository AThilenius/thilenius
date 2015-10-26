// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/server/billet_handler.h"

#include <gflags/gflags.h>

#include "base/directory.h"
#include "base/file.h"
#include "base/guid.h"
#include "base/log.h"
#include "base/path.h"
#include "base/process.h"
#include "base/string.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_client.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/billet/docker_run_harness.h"
#include "scorch/cloud/crucible/Crucible.h"
#include "utils/differencer/differencer.h"

DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");
DEFINE_string(fiber_ip, "localhost", "The Fiber instance ip address.");
DEFINE_int32(fiber_port, 2600, "The Fiber instance port number.");
DEFINE_string(fiber_route, "/", "The Fiber instance route.");
DEFINE_string(crucible_ip, "localhost",
              "The ip enpoint of the Crucible server");
DEFINE_int32(crucible_port, 2200, "The port for the Crucible server");
DEFINE_string(crucible_route, "/", "The route of the Crucible server");
DEFINE_string(session_files_root_path, "/billet",
              "The path of the root folder for local Billet sessions");

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Guid;
using ::thilenius::base::Path;
using ::thilenius::base::Process;
using ::thilenius::base::String;
using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelClient;
using ::thilenius::cloud::utils::ThriftHttpClient;
using ::thilenius::scorch::cloud::billet::DockerRunHarness;
using ::thilenius::utils::differencer::Differencer;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {
namespace server {
namespace {

// For now, mapping:
// /billet/<USER_UUID>/<REPO_UUID>
// And mounting:
// /billet/<USER_UUID> => /billet

void ThrowOpFailure(const std::string& message) {
  ::billet::proto::OperationFailure op_failure;
  op_failure.user_message = message;
  throw op_failure;
}

void AuthenticateOrThrow(const ::sentinel::proto::Token& token) {
  // Authenticate
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route);
  if (!sentinel_client.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

ValueOf<void> BringRepoToCL(const std::string& repo_path,
                            const ::crucible::proto::RepoHeader& repo_header) {
  if (!Directory::Exists(repo_path)) {
    Directory::Create(repo_path);
  }
  Differencer differencer;
  // Establish an ephemeral connection to Crucible
  ThriftHttpClient<::crucible::proto::CrucibleClient>
      ephemeral_crucible_connection(FLAGS_crucible_ip, FLAGS_crucible_port,
                                    FLAGS_crucible_route);
  auto connection_value = ephemeral_crucible_connection.Connect();
  if (!connection_value.IsValid()) {
    return {
        StrCat("Failed to connect to Crucible: ", connection_value.GetError())};
  }
  auto connection = connection_value.GetOrDie();
  // Pull the full repo down every time
  // TODO(athilenius): Don't do that
  ::crucible::proto::Repo repo_proto;
  try {
    connection->GetRepoById(repo_proto, repo_header.repo_uuid);
  } catch (::crucible::proto::OperationFailure op_failure) {
    return {StrCat("Crucible remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {
        "In BilletSession::BringRepoToCL, Crucible server threw an unhandled "
        "exception."};
  }
  // Don't bother to write it to disk, just compute the files in-memory. First
  // compute files up to the latest CL in the given RepoHeader
  std::map<std::string, ::crucible::proto::File> active_files;
  for (const auto& change_list : repo_proto.change_lists) {
    for (const auto& file : change_list.added_files) {
      active_files[file.file_info.relative_path] = file;
    }
    for (const auto& file_delta : change_list.modified_files) {
      ::crucible::proto::File& file =
          active_files[file_delta.file_info.relative_path];
      file.source = differencer.ApplyPatches(file.source, file_delta.patches);
    }
    for (const auto& file_info : change_list.removed_files) {
      auto iter = active_files.find(file_info.relative_path);
      active_files.erase(iter);
    }
    if (change_list.change_list_uuid == repo_header.latest_change_list_uuid) {
      break;
    }
    // TODO(athilenius): Check that we actually found the CL and didn't walk off
    // the edge of the CL list
  }
  // Finally, write the files to disk iff they changed contents
  for (const auto& file : active_files) {
    std::string full_path = Path::Combine(repo_path, file.first);
    if (File::Exists(full_path) &&
        File::ReadContentsOrDie(full_path) != file.second.source) {
      if (!File::WriteToFile(full_path, file.second.source)) {
        LOG(WARNING) << "Failed to write file " << full_path;
      }
    } else {
      if (!File::WriteToFile(full_path, file.second.source)) {
        LOG(WARNING) << "Failed to write file " << full_path;
      }
    }
  }
  return {};
}

}  // namespace

BilletHandler::BilletHandler() {}

void BilletHandler::SyncAndExec(
    ::fiber::proto::Cord& _return, const ::sentinel::proto::Token& token,
    const ::crucible::proto::RepoHeader& repo_header,
    const std::string& shell_command) {
  AuthenticateOrThrow(token);
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = sessions_.find(token.user_uuid);
    if (iter != sessions_.end() &&
        iter->second->GetSessionStatus().is_running) {
      throw ::billet::proto::SessionBusy();
    }
    // Release the lock again, bceause syncing a CL can take a while
  }
  std::string repo_path = Path::Combine(
      Path::Combine(FLAGS_session_files_root_path, token.user_uuid),
      repo_header.repo_uuid);
  ValueOf<void> repo_sync_value = BringRepoToCL(repo_path, repo_header);
  if (!repo_sync_value.IsValid()) {
    ThrowOpFailure(StrCat("Failed to sync repo ", repo_header.repo_uuid, ": ",
                          repo_sync_value.GetError()));
  }
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = sessions_.find(token.user_uuid);
    BilletSessionPtr session_ptr = nullptr;
    if (iter == sessions_.end()) {
      session_ptr = BilletSession::CreateSession(
          FLAGS_fiber_ip, FLAGS_fiber_port, FLAGS_fiber_route);
      sessions_[token.user_uuid] = session_ptr;
    } else {
      session_ptr = iter->second;
    }
    ValueOf<::fiber::proto::Cord> cord_value =
        session_ptr->HarnessRunAndDetatch(token, repo_path, shell_command);
    if (!cord_value.IsValid()) {
      ThrowOpFailure(cord_value.GetError());
    }
    _return = cord_value.GetOrDie();
  }
}

void BilletHandler::TerminateSession(const ::sentinel::proto::Token& token) {
  std::unique_lock<std::mutex> lock(mutex_);
  auto iter = sessions_.find(token.user_uuid);
  if (iter != sessions_.end()) {
    iter->second->TerminateSession();
  }
}

void BilletHandler::GetSessionStatus(::billet::proto::SessionStatus& _return,
                                     const ::sentinel::proto::Token& token) {
  AuthenticateOrThrow(token);
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = sessions_.find(token.user_uuid);
    if (iter == sessions_.end()) {
      ::billet::proto::SessionStatus session_status;
      session_status.is_running = false;
      _return = std::move(session_status);
    } else {
      _return = iter->second->GetSessionStatus();
    }
  }
}

void BilletHandler::ClangFormat(std::string& _return,
                                const std::string& source) {
  // TODO(athilenius): Use CIN, not a file. This is some hacky shit right here
  std::string tmp_path = "/billet/clang_format/" + Guid::NewGuid() + ".cc";
  if (!Directory::Exists("/billet/clang_format")) {
    if (!Directory::Create("/billet/clang_format")) {
      ThrowOpFailure("Failed to create directory");
    }
  }
  if (!File::WriteToFile(tmp_path, source)) {
    ThrowOpFailure("Failed to write to file");
  }
  std::string clang_format_path = Process::FindExecutable("clang-format-3.6");
  ProcessPtr process =
      Process::FromExecv(clang_format_path, {"-style=Google", tmp_path});
  if (!process->Execute(true, 1000)) {
    File::Remove(tmp_path);
    ThrowOpFailure("Failed to execute clang-format-3.6");
  }
  File::Remove(tmp_path);
  auto app_lines_value = process->ReadOutputAfterIndex(0);
  if (!app_lines_value.IsValid()) {
    ThrowOpFailure("Failed to execute clang-format-3.6");
  }
  auto app_lines = app_lines_value.GetOrDie();
  std::string full_content;
  for (const auto& line : app_lines) {
    full_content.append(line.content);
  }
  _return = std::move(full_content);
}

}  // namespace server
}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
