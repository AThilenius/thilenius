// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/server/crucible_handler.h"
#include <algorithm>
#include <gflags/gflags.h>

#include "base/guid.h"
#include "base/json.h"
#include "base/log.h"
#include "base/string.h"
#include "base/time.h"
#include "cloud/sentinel/sentinel_client.h"
#include "cloud/sentinel/sentinel_constants.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_model.h"
#include "scorch/cloud/crucible/crucible_types.h"

DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");

using ::thilenius::base::Guid;
using ::thilenius::base::String;
using ::thilenius::base::Time;
using ::thilenius::cloud::sentinel::SentinelClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace server {

CrucibleHandler::CrucibleHandler() {}

void CrucibleHandler::CreateBaseRepo(
    ::crucible::proto::Repo& _return,
    const ::sentinel::proto::Token& user_stoken, const std::string& repo_name) {
  CrucibleModel model;
  AuthenticateOrThrow(user_stoken);
  if (user_stoken.permission_level <
      ::sentinel::proto::g_sentinel_constants.ADMIN_THRESHOLD) {
    ThrowOpFailure(
        "You do not have sufficient permissions to create a base repo");
  }
  // Make sure the repo doesn't already exit
  ::crucible::proto::Repo repo;
  if (model.FindRepoByRepoName(&repo, repo_name)) {
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
    model.SaveRepo(repo);
    _return = std::move(repo);
  }
}

void CrucibleHandler::CreateForkedRepo(
    ::crucible::proto::Repo& _return,
    const ::sentinel::proto::Token& user_stoken,
    const std::string& base_repo_name) {
  CrucibleModel model;
  AuthenticateOrThrow(user_stoken);
  if (user_stoken.permission_level <
      ::sentinel::proto::g_sentinel_constants.USER_THRESHOLD) {
    ThrowOpFailure(
        "You do not have sufficient permissions to create a cloned repo");
  }
  std::string repo_name = StrCat(base_repo_name, "/", user_stoken.user_uuid);
  ::crucible::proto::Repo repo;
  // Check if the repo was already created
  if (model.FindRepoByRepoName(&repo, repo_name)) {
    _return = std::move(repo);
    return;
  }
  // Find the base repo
  if (model.FindRepoByRepoName(&repo, base_repo_name)) {
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
    model.SaveRepo(cloned_repo);
    _return = std::move(cloned_repo);
  } else {
    ThrowOpFailure(
        StrCat("Failed to find a base repo with the name ", base_repo_name));
  }
}

void CrucibleHandler::GetRepoHeadersByUser(
    std::vector<::crucible::proto::RepoHeader>& _return,
    const ::sentinel::proto::Token& user_stoken) {
  CrucibleModel model;
  AuthenticateOrThrow(user_stoken);
  std::vector<::crucible::proto::RepoHeader> users_repos =
      model.FindRepoHeadersByUserId(user_stoken.user_uuid);
  if (user_stoken.permission_level >=
      ::sentinel::proto::g_sentinel_constants.ADMIN_THRESHOLD) {
    _return = std::move(users_repos);
    return;
  }
  std::unordered_map<std::string, const ::crucible::proto::RepoHeader&> lookup;
  for (const auto& user_repo : users_repos) {
    if (!String::Blank(user_repo.base_repo_uuid)) {
      lookup.insert({user_repo.base_repo_uuid, user_repo});
    }
  }
  std::vector<::crucible::proto::RepoHeader> all_base_repos =
      model.FindAllBaseRepos();
  for (const auto& base_repo : all_base_repos) {
    auto iter = lookup.find(base_repo.repo_uuid);
    if (iter == lookup.end()) {
      // User doesn't have this base repo clones, clone it.
      std::string repo_name =
          StrCat(base_repo.repo_name, "/", user_stoken.user_uuid);
      // Load up the full repo (We need it's change lists)
      ::crucible::proto::Repo repo;
      if (model.FindRepoByRepoName(&repo, base_repo.repo_name)) {
        // Create a new cloned repo from the base
        ::crucible::proto::RepoHeader repo_header;
        repo_header.repo_uuid = Guid::NewGuid();
        repo_header.user_uuid = user_stoken.user_uuid;
        repo_header.repo_name = repo_name;
        repo_header.creation_timestamp =
            std::to_string(Time::EpochMilliseconds());
        repo_header.base_repo_uuid = repo.repo_header.repo_uuid;
        ::crucible::proto::Repo cloned_repo;
        // Copy change list from base
        cloned_repo.change_lists = std::move(repo.change_lists);
        cloned_repo.repo_header = std::move(repo_header);
        model.SaveRepo(cloned_repo);
        users_repos.emplace_back(cloned_repo.repo_header);
      } else {
        // Not sure what to do with this. I guess ignore that repo?
      }
    }
  }
  _return = std::move(users_repos);
}

void CrucibleHandler::GetRepoById(::crucible::proto::Repo& _return,
                                  const ::sentinel::proto::Token& user_stoken,
                                  const std::string& repo_uuid) {
  CrucibleModel model;
  AuthenticateOrThrow(user_stoken);
  ::crucible::proto::Repo repo;
  if (!model.FindRepoById(&repo, repo_uuid)) {
    ThrowOpFailure(StrCat("Failed to find repo with ID: ", repo_uuid));
  }
  _return = std::move(repo);
}

void CrucibleHandler::GetRepoHeaderById(
    ::crucible::proto::RepoHeader& _return,
    const ::sentinel::proto::Token& user_stoken, const std::string& repo_uuid) {
  CrucibleModel model;
  AuthenticateOrThrow(user_stoken);
  ::crucible::proto::Repo repo;
  if (!model.FindRepoById(&repo, repo_uuid)) {
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
  CrucibleModel model;
  AuthenticateOrThrow(user_stoken);
  if (user_stoken.permission_level <
      ::sentinel::proto::g_sentinel_constants.SECONDARY_THRESHOLD) {
    ThrowOpFailure("Insufficient permission level to commit to a repo");
  }
  ::crucible::proto::Repo repo;
  if (!model.FindRepoById(&repo, repo_header.repo_uuid)) {
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
      // TODO(need to filter the files by is_locked)
      ::crucible::proto::ChangeList new_change_list = change_list;
      new_change_list.change_list_uuid = Guid::NewGuid();
      new_change_list.timestamp = std::to_string(Time::EpochMilliseconds());
      repo.change_lists.push_back(new_change_list);
      repo.repo_header.latest_change_list_uuid =
          new_change_list.change_list_uuid;
      model.SaveRepo(repo);
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
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route);
  // Authenticate
  if (!sentinel_client.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

}  // namespace server
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
