// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/server/billet_handler.h"

#include <gflags/gflags.h>

#include "base/directory.h"
#include "base/file.h"
#include "base/guid.h"
#include "base/log.h"
#include "base/process.h"
#include "base/string.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_client.h"

DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Guid;
using ::thilenius::base::Process;
using ::thilenius::base::String;
using ::thilenius::base::ValueOf;
using ::thilenius::cloud::sentinel::SentinelClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {
namespace server {

BilletHandler::BilletHandler() {}

void BilletHandler::CreateSession(
    ::billet::proto::Session& _return,
    const ::sentinel::proto::Token& sentinel_token) {
  AuthenticateOrThrow(sentinel_token);
  if (sessions_.find(sentinel_token.user_uuid) != sessions_.end()) {
    _return = sessions_[sentinel_token.user_uuid].billet_session_proto;
    return;
  }
  ::billet::proto::Session billet_session_proto;
  billet_session_proto.uuid = Guid::NewGuid();
  billet_session_proto.session_key = sentinel_token;
  sessions_[sentinel_token.user_uuid] = BilletSession(billet_session_proto);
  _return = billet_session_proto;
}

void BilletHandler::BuildCMakeRepo(
    const ::billet::proto::Session& session,
    const ::crucible::proto::RepoHeader& repo_header,
    const std::vector<::crucible::proto::ChangeList>& staged_change_lists,
    const std::vector<std::string>& application_args) {
  AuthenticateOrThrow(session.session_key);
  if (sessions_.find(session.session_key.user_uuid) == sessions_.end()) {
    ThrowOpFailure("You do not have an active session");
  }
  auto& user_session = sessions_[session.session_key.user_uuid];
  ValueOf<void> execute_value = user_session.CompileCMakeRepo(
      repo_header, staged_change_lists, application_args);
  if (!execute_value.IsValid()) {
    ThrowOpFailure(
        StrCat("Failed to execute CMake repo: ", execute_value.GetError()));
  }
}

void BilletHandler::RunRepo (const ::billet::proto::Session& session) {}

void BilletHandler::QueryCompilerOutputAfterLine(
    ::billet::proto::ApplicationOutput& _return,
    const ::billet::proto::Session& session, const int32_t line) {
  AuthenticateOrThrow(session.session_key);
  if (sessions_.find(session.session_key.user_uuid) == sessions_.end()) {
    ThrowOpFailure("You do not have an active application");
  }
  auto& user_session = sessions_[session.session_key.user_uuid];
  ValueOf<::billet::proto::ApplicationOutput> output_value =
      user_session.GetCompilerOutputTillLine(line);
  if (!output_value.IsValid()) {
    ThrowOpFailure(StrCat("Failed to get output: ", output_value.GetError()));
  }
  _return = std::move(output_value.GetOrDie());
}

void BilletHandler::QueryApplicationOutputAfterLine(
      ::billet::proto::ApplicationOutput& _return,
      const ::billet::proto::Session& session, const int32_t line) {}

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
  //std::string clang_format_path = Process::FindExecutable("clang-format-3.6");
  //ProcessPtr process =
      //Process::FromExecv(clang_format_path, {"-style=Google", tmp_path});
  //if (!process->Execute(true, 1000)) {
    //File::Remove(tmp_path);
    //ThrowOpFailure("Failed to execute clang-format-3.6");
  //}
  //File::Remove(tmp_path);
  //_return = process->ReadOutputAfterIndex(0).GetOrDie().;
}

void BilletHandler::ThrowOpFailure(const std::string& message) {
  ::billet::proto::OperationFailure op_failure;
  op_failure.user_message = message;
  throw op_failure;
}

void BilletHandler::AuthenticateOrThrow(const ::sentinel::proto::Token& token) {
  // Authenticate
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route);
  if (!sentinel_client.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

}  // namespace server
}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
