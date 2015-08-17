// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_handler.h"

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "base/string.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"

DEFINE_string(mongo_ip, "192.168.59.103", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");

using ::thilenius::base::StrCat;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

// TABLE - repos
// STRUCTURE -
// {
//   repo_uuid,
//   base_repo_uuid,
//   user_uuid,
//   creation_timestamp
//   commits: [
//     {
//       commit_uuid,
//       user_uuid,
//       timestamp,
//       added_files: [ FILE.. ],
//       modified_files: [ FILE... ],
//       removed_files: [ FILE... ]
//     }, ...
//   ]
// }

CrucibleHandler::CrucibleHandler() {
  try {
    LOG(INFO) << "Connecting to MongoDB at " << FLAGS_mongo_ip << ":"
              << FLAGS_mongo_port;
    mongo_connection_.connect(StrCat(FLAGS_mongo_ip, ":", FLAGS_mongo_port));
  } catch (const mongo::DBException& e) {
    LOG(FATAL) << "MongoDB Driver failed to connect to " << FLAGS_mongo_ip
               << ":" << FLAGS_mongo_port;
  }
}

void CrucibleHandler::CreateNewRepo(::crucible::RepoInfo& _return,
                                    const std::string& user_uuid,
                                    const std::string& repo_name) {
  // Your implementation goes here
  LOG(INFO) << "CreateNewRepo";
  _return = ::crucible::RepoInfo();
}

void CrucibleHandler::CreateForkedRepo(::crucible::RepoInfo& _return,
                                       const std::string& user_uuid,
                                       const std::string& base_repo_uuid) {
  // Your implementation goes here
  LOG(INFO) << "CreateForkedRepo";
}

void CrucibleHandler::GetAllRepoInfo(std::vector<::crucible::RepoInfo>& _return,
                                     const std::string& user_uuid) {
  // Your implementation goes here
  LOG(INFO) << "GetAllRepoInfo";
}

void CrucibleHandler::CommitAndDownstream(
    ::crucible::RepoInfo& _return, const std::string& repo_uuid,
    const ::crucible::ChangeList& change_list) {
  // Your implementation goes here
  LOG(INFO) << "CommitAndDownstream";
}

void CrucibleHandler::GetRepo(::crucible::Repo& _return,
                              const std::string& repo_uuid) {
  // Your implementation goes here
  LOG(INFO) << "GetRepo";
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
