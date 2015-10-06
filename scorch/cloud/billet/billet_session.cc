// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/billet_session.h"

#include <gflags/gflags.h>

#include "base/directory.h"
#include "base/file.h"
#include "base/log.h"
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
using ::thilenius::base::Process;
using ::thilenius::base::String;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

BilletSession::BilletSession(
    const ::billet::proto::Session& billet_session_proto)
    : session_state_(SessionState::IDLE),
      billet_session_proto(billet_session_proto) {}

ValueOf<void> BilletSession::CompileCMakeRepo(
    const ::crucible::proto::RepoHeader& repo_header_proto,
    const std::vector<::crucible::proto::ChangeList>& staged_change_list,
    const std::vector<std::string>& application_args) {
  if (session_state_ != SessionState::IDLE &&
      session_state_ <= SessionState::RUNNING) {
    return {"A task is already running"};
  }
  session_state_ = SessionState::COMPILING;
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
  std::string build_path = Path::Combine(repo_path, "build");
  if (!Directory::Exists(build_path) && !Directory::Create(build_path)) {
    return {StrCat("Failed to create directory ", build_path)};
  };
  std::string bash_path = Process::FindExecutable("bash");
  std::string executable_path = Path::Combine(build_path, "runnable");
  std::string cmake_bash_command = StrCat(
      "export CC=/usr/bin/clang-3.6;", "export CXX=/usr/bin/clang++-3.6;",
      "cd ", build_path, ";", "cmake .. && make");
  compiler_process_ = Process::FromExecv(bash_path, {"-c", cmake_bash_command});
  execute_process_ = Process::FromExecv(executable_path, application_args);
  if (!compiler_process_->Execute(false, 120000)) {
    return {"Failed to start child process"};
  }
  return {};
}

ValueOf<void> BilletSession::ExecuteCMakeRepo() {
  if (session_state_ != SessionState::COMPILATION_DONE) {
    return {"Compilation still going on, or stream not read to end"};
  }
  if (!execute_process_->Execute(false, 120000)) {
    return {"Failed to start child process"};
  }
  return {};
}

ValueOf<::billet::proto::ApplicationOutput>
BilletSession::GetCompilerOutputTillLine(int line) {
  auto ostream_token_value = compiler_process_->ReadOutputAfterIndex(line);
  if (!ostream_token_value.IsValid()) {
    session_state_ = SessionState::COMPILATION_DONE;
    ::billet::proto::ApplicationOutput application_output;
    application_output.did_terminate = true;
    // TODO(athilenius): Not sure how to get at this yet
    application_output.termination_code = 0;
  }
  ::billet::proto::ApplicationOutput application_output;
  auto ostream_tokens_value = ostream_token_value;
  if (!ostream_tokens_value.IsValid()) {
    application_output.did_terminate = true;
    session_state_ = SessionState::IDLE;
    return std::move(application_output);
  }
  auto ostream_tokens = ostream_tokens_value.GetOrDie();
  application_output.did_terminate = false;
  for (const auto& ostream_token : ostream_tokens) {
    ::billet::proto::OutputToken output_token;
    output_token.is_cerr = ostream_token.is_err_stream;
    output_token.content = ostream_token.content;
    application_output.output_tokens.push_back(std::move(output_token));
  }
  return std::move(application_output);
}

ValueOf<::billet::proto::ApplicationOutput>
BilletSession::GetApplicationOutputTillLine(int line) {
  auto ostream_token_value = execute_process_->ReadOutputAfterIndex(line);
  if (!ostream_token_value.IsValid()) {
    session_state_ = SessionState::RUNNING_DONE;
    ::billet::proto::ApplicationOutput application_output;
    application_output.did_terminate = true;
    // TODO(athilenius): Not sure how to get at this yet
    application_output.termination_code = 0;
  }
  ::billet::proto::ApplicationOutput application_output;
  std::vector<Process::OStreamToken> ostream_tokens =
      ostream_token_value.GetOrDie();
  application_output.did_terminate = false;
  for (const auto& ostream_token : ostream_tokens) {
    ::billet::proto::OutputToken output_token;
    output_token.is_cerr = ostream_token.is_err_stream;
    output_token.content = ostream_token.content;
    application_output.output_tokens.push_back(std::move(output_token));
  }
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
