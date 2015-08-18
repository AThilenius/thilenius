// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_handler.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "base/gflags/gflags.h"
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

using ::thilenius::base::Time;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

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
    _return = std::move(repo);
  } else {
    // Create a new one
    ::crucible::RepoHeader repo_header;
    repo_header.repo_uuid =
        boost::lexical_cast<std::string>(::boost::uuids::random_generator()());
    repo_header.user_uuid = user_uuid;
    repo_header.repo_name = repo_name;
    repo_header.creation_timestamp = Time::EpochMilliseconds();
    repo = std::move(::crucible::Repo());
    repo.repo_header = std::move(repo_header);
    model_.SaveRepo(repo);
    _return = repo;
  }
  LOG(INFO) << "CreateNewRepo returning: "
            << CrucibleModel::RepoToJson(_return).dump(2);
}

void CrucibleHandler::CreateForkedRepo(::crucible::Repo& _return,
                                       const std::string& user_uuid,
                                       const std::string& base_repo_uuid) {
  // Your implementation goes here
  printf("CreateForkedRepo\n");
}

void CrucibleHandler::GetRepoHeadersByUser(
    std::vector<::crucible::RepoHeader>& _return,
    const std::string& user_uuid) {
  // Your implementation goes here
  printf("GetRepoHeadersByUser\n");
}

void CrucibleHandler::GetRepoById(::crucible::Repo& _return,
                                  const std::string& repo_uuid) {
  // Your implementation goes here
  printf("GetRepoById\n");
}

void CrucibleHandler::CommitAndDownstream(
    ::crucible::ChangeList& _return, const std::string& repo_uuid,
    const ::crucible::ChangeList& change_list) {
  // Your implementation goes here
  printf("CommitAndDownstream\n");
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
