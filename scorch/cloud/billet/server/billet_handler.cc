// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/billet/server/billet_handler.h"

#include <gflags/gflags.h>

#include "base/string.h"
#include "base/guid.h"
#include "base/log.h"
#include "base/value_of.hh"

DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");

using ::thilenius::base::Guid;
using ::thilenius::base::String;
using ::thilenius::base::ValueOf;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {
namespace server {

BilletHandler::BilletHandler() {
  LOG(INFO) << "Connecting to Sentinel at " << FLAGS_sentinel_ip << ":"
            << FLAGS_sentinel_port;
  sentinel_client_.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                           FLAGS_sentinel_route).GetOrDie();
}

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
  sessions_[sentinel_token.user_uuid] =
      BilletSession::CreateLocalSession(billet_session_proto);
  _return = billet_session_proto;
}

void BilletHandler::ExecuteCMakeRepo(
    const ::billet::proto::Session& session,
    const ::crucible::proto::RepoHeader& repo_header,
    const std::vector<::crucible::proto::ChangeList>& staged_change_lists,
    const std::vector<std::string>& application_args) {
  AuthenticateOrThrow(session.session_key);
  if (sessions_.find(session.session_key.user_uuid) == sessions_.end()) {
    ThrowOpFailure("You do not have an active session");
  }
  auto& user_session = sessions_[session.session_key.user_uuid];
  ValueOf<void> execute_value = user_session.ExecuteCMakeRepo(
      repo_header, staged_change_lists, application_args);
  if (!execute_value.IsValid()) {
    ThrowOpFailure(
        StrCat("Failed to execute CMake repo: ", execute_value.GetError()));
  }
}

void BilletHandler::QueryOutputAfterLine(
    ::billet::proto::ApplicationOutput& _return,
    const ::billet::proto::Session& session, const int32_t line) {
  AuthenticateOrThrow(session.session_key);
  if (sessions_.find(session.session_key.user_uuid) == sessions_.end()) {
    ThrowOpFailure("You do not have an active session");
  }
  auto& user_session = sessions_[session.session_key.user_uuid];
  ValueOf<::billet::proto::ApplicationOutput> output_value =
      user_session.GetOutputTillLine(line);
  if (!output_value.IsValid()) {
    ThrowOpFailure(
        StrCat("Failed to get output: ", output_value.GetError()));
  }
  _return = std::move(output_value.GetOrDie());
}

void BilletHandler::ThrowOpFailure(const std::string& message) {
  ::billet::proto::OperationFailure op_failure;
  op_failure.user_message = message;
  throw op_failure;
}

void BilletHandler::AuthenticateOrThrow(const ::sentinel::proto::Token& token) {
  // Authenticate
  if (!sentinel_client_.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

}  // namespace server
}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
