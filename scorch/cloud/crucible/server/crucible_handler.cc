// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/server/crucible_handler.h"

#include <gflags/gflags.h>

#include "base/guid.h"
#include "base/json.h"
#include "base/log.h"
#include "base/string.h"
#include "base/time.h"
#include "cloud/sentinel/sentinel_constants.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"

DEFINE_string(mongo_ip, "192.168.59.103", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");
DEFINE_string(mongo_table, "crucible.repos",
              "The MongoDB database and table name to use.");
DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");

using ::thilenius::base::Guid;
using ::thilenius::base::Time;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace server {

CrucibleHandler::CrucibleHandler()
    : model_(mongo_connection_, FLAGS_mongo_table) {
  try {
    LOG(INFO) << "Connecting to MongoDB at " << FLAGS_mongo_ip << ":"
              << FLAGS_mongo_port;
    mongo_connection_.connect(StrCat(FLAGS_mongo_ip, ":", FLAGS_mongo_port));
  } catch (const mongo::DBException& e) {
    LOG(FATAL) << "MongoDB Driver failed to connect to " << FLAGS_mongo_ip
               << ":" << FLAGS_mongo_port;
  }
  LOG(INFO) << "Connecting to Sentinel at " << FLAGS_sentinel_ip << ":"
            << FLAGS_sentinel_port;
  sentinel_client_.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                           FLAGS_sentinel_route).GetOrDie();
}

void CrucibleHandler::CreateBaseRepo(
    ::crucible::proto::Repo& _return,
    const ::sentinel::proto::Token& user_stoken, const std::string& repo_name) {
  AuthenticateOrThrow(user_stoken);
  if (user_stoken.permission_level <
      ::sentinel::proto::g_sentinel_constants.ADMIN_THRESHOLD) {
    ThrowOpFailure(
        "You do not have sufficient permissions to create a base repo");
  }
  // Make sure the repo doesn't already exit
  ::crucible::proto::Repo repo;
  if (model_.FindRepoByRepoName(&repo, repo_name)) {
    _return = std::move(repo);
    return;
  } else {
    // Create a new one
    ::crucible::proto::RepoHeader repo_header;
    repo_header.repo_uuid = Guid::NewGuid();
    repo_header.user_uuid = user_stoken.user_uuid;
    repo_header.repo_name = repo_name;
    repo_header.creation_timestamp = std::to_string(Time::EpochMilliseconds());
    repo = std::move(::crucible::proto::Repo());
    repo.repo_header = std::move(repo_header);
    model_.SaveRepo(repo);
    _return = std::move(repo);
  }
}

void CrucibleHandler::CreateForkedRepo(
    ::crucible::proto::Repo& _return,
    const ::sentinel::proto::Token& user_stoken,
    const std::string& base_repo_name) {
  AuthenticateOrThrow(user_stoken);
  if (user_stoken.permission_level <
      ::sentinel::proto::g_sentinel_constants.USER_THRESHOLD) {
    ThrowOpFailure(
        "You do not have sufficient permissions to create a cloned repo");
  }
  std::string repo_name = StrCat(base_repo_name, "/", user_stoken.user_uuid);
  ::crucible::proto::Repo repo;
  // Check if the repo was already created
  if (model_.FindRepoByRepoName(&repo, repo_name)) {
    _return = std::move(repo);
    return;
  }
  // Find the base repo
  if (model_.FindRepoByRepoName(&repo, base_repo_name)) {
    // Create a new cloned repo from the base
    ::crucible::proto::RepoHeader repo_header;
    repo_header.repo_uuid = Guid::NewGuid();
    repo_header.user_uuid = user_stoken.user_uuid;
    repo_header.repo_name = repo_name;
    repo_header.creation_timestamp = std::to_string(Time::EpochMilliseconds());
    repo_header.base_repo_uuid = repo.repo_header.repo_uuid;
    ::crucible::proto::Repo cloned_repo;
    // Copy change list from base
    cloned_repo.change_lists = std::move(repo.change_lists);
    cloned_repo.repo_header = std::move(repo_header);
    model_.SaveRepo(cloned_repo);
    _return = std::move(cloned_repo);
  } else {
    ThrowOpFailure(
        StrCat("Failed to find a base repo with the name ", base_repo_name));
  }
}

void CrucibleHandler::GetRepoHeadersByUser(
    std::vector<::crucible::proto::RepoHeader>& _return,
    const ::sentinel::proto::Token& user_stoken) {
  AuthenticateOrThrow(user_stoken);
  _return = std::move(model_.FindRepoHeadersByUserId(user_stoken.user_uuid));
}

void CrucibleHandler::GetRepoById(::crucible::proto::Repo& _return,
                                  const ::sentinel::proto::Token& user_stoken,
                                  const std::string& repo_uuid) {
  AuthenticateOrThrow(user_stoken);
  ::crucible::proto::Repo repo;
  if (!model_.FindRepoById(&repo, repo_uuid)) {
    ThrowOpFailure(StrCat("Failed to find repo with ID: ", repo_uuid));
  }
  _return = std::move(repo);
}

void CrucibleHandler::GetRepoHeaderById(
    ::crucible::proto::RepoHeader& _return,
    const ::sentinel::proto::Token& user_stoken, const std::string& repo_uuid) {
  AuthenticateOrThrow(user_stoken);
  ::crucible::proto::Repo repo;
  if (!model_.FindRepoById(&repo, repo_uuid)) {
    ThrowOpFailure(StrCat("Failed to find a repo with ID: ", repo_uuid));
  } else {
    _return = repo.repo_header;
  }
}

void CrucibleHandler::CommitAndDownstream(
    ::crucible::proto::ChangeList& _return,
    const ::sentinel::proto::Token& user_stoken,
    const ::crucible::proto::RepoHeader& repo_header,
    const ::crucible::proto::ChangeList& change_list) {
  AuthenticateOrThrow(user_stoken);
  if (user_stoken.permission_level <
      ::sentinel::proto::g_sentinel_constants.SECONDARY_THRESHOLD) {
    ThrowOpFailure("Insufficient permission level to commit to a repo");
  }
  ::crucible::proto::Repo repo;
  if (!model_.FindRepoById(&repo, repo_header.repo_uuid)) {
    ThrowOpFailure(
        StrCat("Failed to find a repo with ID: ", repo_header.repo_uuid));
  } else {
    if (repo_header.user_uuid != user_stoken.user_uuid) {
      ThrowOpFailure("You do not own that repo");
    }
    if (repo_header.latest_change_list_uuid !=
        repo.repo_header.latest_change_list_uuid) {
      ThrowOpFailure(
          "Your repo is behind head. You must sync before committing.");
    } else {
      // TODO(athilenius) Need to downstream as well
      ::crucible::proto::ChangeList new_change_list = change_list;
      new_change_list.change_list_uuid = Guid::NewGuid();
      new_change_list.timestamp = std::to_string(Time::EpochMilliseconds());
      repo.change_lists.push_back(new_change_list);
      repo.repo_header.latest_change_list_uuid =
          new_change_list.change_list_uuid;
      model_.SaveRepo(repo);
      _return = std::move(new_change_list);
    }
  }
}

void CrucibleHandler::ThrowOpFailure(const std::string& message) {
  ::crucible::proto::OperationFailure op_failure;
  op_failure.user_message = message;
  throw op_failure;
}

void CrucibleHandler::AuthenticateOrThrow(
    const ::sentinel::proto::Token& token) {
  // Authenticate
  if (!sentinel_client_.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

}  // namespace server
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
