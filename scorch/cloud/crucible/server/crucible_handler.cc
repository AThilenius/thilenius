// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/server/crucible_handler.h"

#include "base/gflags/gflags.h"
#include "base/guid.h"
#include "base/json.h"
#include "base/log.h"
#include "base/string.h"
#include "base/time.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"

DEFINE_string(mongo_ip, "192.168.59.103", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");
DEFINE_string(mongo_table, "crucible.repos",
              "The MongoDB database and table name to use.");

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
}

void CrucibleHandler::CreateNewRepo(::crucible::Repo& _return,
                                    const std::string& user_uuid,
                                    const std::string& repo_name) {
  // Make sure the repo doesn't already exit
  ::crucible::Repo repo;
  if (model_.FindRepoByUserIdAndRepoName(&repo, user_uuid, repo_name)) {
    ::crucible::OperationFailure op_failure;
    op_failure.user_message =
        "A repo with that username and repo name already exists";
    throw op_failure;
  } else {
    // Create a new one
    ::crucible::RepoHeader repo_header;
    repo_header.repo_uuid = Guid::NewGuid();
    repo_header.user_uuid = user_uuid;
    repo_header.repo_name = repo_name;
    repo_header.creation_timestamp = std::to_string(Time::EpochMilliseconds());
    repo = std::move(::crucible::Repo());
    repo.repo_header = std::move(repo_header);
    model_.SaveRepo(repo);
    _return = repo;
  }
}

void CrucibleHandler::CreateForkedRepo(::crucible::Repo& _return,
                                       const std::string& user_uuid,
                                       const std::string& base_repo_uuid) {
  // Your implementation goes here
  printf("CreateForkedRepo\n");
}

void CrucibleHandler::GetRepoHeadersByUser(
    std::vector<::crucible::RepoHeader>& _return,
    const std::string& user_uuid) {}

void CrucibleHandler::GetRepoById(::crucible::Repo& _return,
                                  const std::string& repo_uuid) {
  // Your implementation goes here
  printf("GetRepoById\n");
}

void CrucibleHandler::GetRepoHeaderById(::crucible::RepoHeader& _return,
                                        const std::string& repo_uuid) {
  ::crucible::Repo repo;
  if (!model_.FindRepoById(&repo, repo_uuid)) {
    ::crucible::OperationFailure op_failure;
    op_failure.user_message =
        StrCat("Failed to find a repo with ID: ", repo_uuid);
    throw op_failure;
  } else {
    _return = repo.repo_header;
  }
}

void CrucibleHandler::CommitAndDownstream(
    ::crucible::ChangeList& _return, const ::crucible::RepoHeader& repo_header,
    const ::crucible::ChangeList& change_list) {
  ::crucible::Repo repo;
  if (!model_.FindRepoById(&repo, repo_header.repo_uuid)) {
    ::crucible::OperationFailure op_failure;
    op_failure.user_message =
        StrCat("Failed to find a repo with ID: ", repo_header.repo_uuid);
    throw op_failure;
  } else {
    if (repo_header.latest_change_list_uuid !=
        repo.repo_header.latest_change_list_uuid) {
      ::crucible::OperationFailure op_failure;
      op_failure.user_message =
          "Your repo is behind head. You must sync before commiting.";
      throw op_failure;
    } else {
      // TODO(athilenius) Need to downstream as well
      ::crucible::ChangeList new_change_list = change_list;
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

}  // namespace server
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
