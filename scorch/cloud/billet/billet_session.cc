// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/billet_session.h"

#include <gflags/gflags.h>

#include "base/directory.h"
#include "base/file.h"
#include "base/log.h"
#include "base/path.h"
#include "base/process.h"
#include "base/string.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/Crucible.h"

DEFINE_string(crucible_ip, "localhost",
              "The ip enpoint of the Crucible server");
DEFINE_int32(crucible_port, 2200, "The port for the Crucible server");
DEFINE_string(crucible_route, "/", "The route of the Crucible server");
DEFINE_string(session_files_root_path, "/billet/local_sessions",
              "The path of the root folder for local Billet sessions");

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::String;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

BilletSession BilletSession::CreateLocalSession(
    const ::billet::proto::Session& billet_session_proto) {
  BilletSession billet_session;
  billet_session.is_running = false;
  billet_session.billet_session_proto = billet_session_proto;
  return std::move(billet_session);
}

ValueOf<void> BilletSession::ExecuteCMakeRepo(
    const ::crucible::proto::RepoHeader& repo_header_proto,
    const std::vector<::crucible::proto::ChangeList>& staged_change_list,
    const std::vector<std::string>& application_args) {
  if (is_running) {
    return {"A task is already running"};
  }
  // Path is: FLAGS_session_files_root_path/<USER_UUID>/<REPO_UUID>/
  std::string repo_path =
      Path::Combine(Path::Combine(FLAGS_session_files_root_path,
                                  billet_session_proto.session_key.user_uuid),
                    repo_header_proto.repo_uuid);
  if (!Directory::Exists(repo_path) && !Directory::Create(repo_path)) {
    return {StrCat("Failed to create directory ", repo_path)};
  }
  ValueOf<void> repo_sync_value =
      BringRepoToCL(repo_path, billet_session_proto.session_key,
                    repo_header_proto, staged_change_list);
  if (!repo_sync_value.IsValid()) {
    return {StrCat("Failed to sync repo ", repo_header_proto.repo_uuid, ": ",
                   repo_sync_value.GetError())};
  }
  // CMake . && make && ./runnable
  std::string bash_path = Process::FindExecutable("bash");
  std::string bash_command = StrCat(
      "export CC=/usr/bin/clang-3.6;", "export CXX=/usr/bin/clang++-3.6;",
      "cd ", repo_path, ";", "mkdir -p build;", "cd build;", "cmake ..;",
      "make;", "./runnable;");
  process_ = std::move(Process::FromExecv("/bin/bash", {"-c", bash_command}));
  if (process_.Execute()) {
    is_running = true;
  } else {
    return {"Failed to start child process"};
  }
  return {};
}

ValueOf<::billet::proto::ApplicationOutput> BilletSession::GetOutputTillLine(
    int line) {
  if (!is_running) {
    return {::billet::proto::ApplicationOutput(), "No tasks are running"};
  }
  // TODO(athilenius): Don't block
  ::billet::proto::ApplicationOutput application_output;
  application_output.did_terminate = true;
  application_output.termination_code = process_.Wait(1000000);
  application_output.standard_out = process_.GetOut();
  application_output.error_out = process_.GetErr();
  is_running = false;
  return std::move(application_output);
}

ValueOf<void> BilletSession::BringRepoToCL(
    const std::string& repo_path, const ::sentinel::proto::Token& token,
    const ::crucible::proto::RepoHeader& repo_header,
    const std::vector<::crucible::proto::ChangeList>& staged_change_list) {
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
    connection->GetRepoById(repo_proto, token, repo_header.repo_uuid);
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
      file.source = differencer_.ApplyPatches(file.source, file_delta.patches);
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
  // Now add in the stages change lists
  for (const auto& change_list : staged_change_list) {
    for (const auto& file : change_list.added_files) {
      active_files[file.file_info.relative_path] = file;
    }
    for (const auto& file_delta : change_list.modified_files) {
      ::crucible::proto::File& file =
          active_files[file_delta.file_info.relative_path];
      file.source = differencer_.ApplyPatches(file.source, file_delta.patches);
    }
    for (const auto& file_info : change_list.removed_files) {
      auto iter = active_files.find(file_info.relative_path);
      active_files.erase(iter);
    }
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

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
