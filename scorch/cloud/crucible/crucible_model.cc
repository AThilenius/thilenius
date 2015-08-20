// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_model.h"

#include "base/json.h"
#include "base/log.h"
#include "base/types.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

bool CrucibleModel::FindRepoById(::crucible::Repo* repo,
                                 const std::string& repo_uuid) {
  ::mongo::BSONObj query = BSON("repo_uuid" << repo_uuid);
  auto cursor = connection_.query(table_, query);
  if (cursor->more()) {
    // Got a repo back
    ::mongo::BSONObj bson = cursor->next();
    bson = bson.removeField("_id");
    *repo =
        crucible_mapper_.JsonToRepo(::nlohmann::json::parse(bson.jsonString()));
    return true;
  } else {
    return false;
  }
}

bool CrucibleModel::FindRepoByUserIdAndRepoName(::crucible::Repo* repo,
                                                const std::string& user_uuid,
                                                const std::string& repo_name) {
  ::mongo::BSONObj query =
      BSON("user_uuid" << user_uuid << "repo_name" << repo_name);
  auto cursor = connection_.query(table_, query);
  if (cursor->more()) {
    // Got a repo back
    ::mongo::BSONObj bson = cursor->next();
    bson = bson.removeField("_id");
    *repo =
        crucible_mapper_.JsonToRepo(::nlohmann::json::parse(bson.jsonString()));
    return true;
  } else {
    return false;
  }
}

std::vector<::crucible::Repo> CrucibleModel::FindReposByUserId(
    const std::string& user_uuid) {
  return std::vector<::crucible::Repo>();
}

std::vector<::crucible::Repo> CrucibleModel::FindReposByRepoBaseId(
    const std::string& base_repo_uuid) {
  return std::vector<::crucible::Repo>();
}

bool CrucibleModel::SaveRepo(const ::crucible::Repo repo) {
  connection_.update(
      table_, BSON("repo_name" << repo.repo_header.repo_name << "user_uuid"
                               << repo.repo_header.user_uuid),
      ::mongo::fromjson(crucible_mapper_.RepoToJson(repo).dump()), true);

  std::string last_error = connection_.getLastError();
  if (last_error != "") {
    LOG(WARNING) << "Got a MongoDB error: " << last_error;
    return false;
  }
  return true;
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
