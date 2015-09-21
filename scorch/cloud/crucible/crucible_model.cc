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

bool CrucibleModel::FindRepoById(::crucible::proto::Repo* repo,
                                 const std::string& repo_uuid) {
  ::mongo::BSONObj query = BSON("repo_header.repo_uuid" << repo_uuid);
  auto cursor = connection_.query(table_, query);
  if (cursor->more()) {
    // Got a repo back
    ::mongo::BSONObj bson = cursor->next();
    crucible_mapper_.repo_mapper.from_bson(bson, *repo);
    return true;
  } else {
    return false;
  }
}

bool CrucibleModel::FindRepoByRepoName(::crucible::proto::Repo* repo,
                                       const std::string& repo_name) {
  ::mongo::BSONObj query = BSON("repo_header.repo_name" << repo_name);
  auto cursor = connection_.query(table_, query);
  if (cursor->more()) {
    // Got a repo back
    ::mongo::BSONObj bson = cursor->next();
    crucible_mapper_.repo_mapper.from_bson(bson, *repo);
    return true;
  } else {
    return false;
  }
}

std::vector<::crucible::proto::RepoHeader>
CrucibleModel::FindRepoHeadersByUserId(const std::string& user_uuid) {
  std::vector<::crucible::proto::RepoHeader> repo_headers;
  ::mongo::BSONObj query = BSON("repo_header.user_uuid" << user_uuid);
  auto cursor = connection_.query(table_, query);
  while (cursor->more()) {
    // Got a repo header back
    ::mongo::BSONObj bson = cursor->next();
    ::crucible::proto::RepoHeader repo_header;
    crucible_mapper_.repo_header_mapper.from_bson(bson, repo_header);
    repo_headers.emplace_back(std::move(repo_header));
  }
  return std::move(repo_headers);
}

std::vector<::crucible::proto::RepoHeader>
CrucibleModel::FindRepoHeadersByRepoBaseId(const std::string& base_repo_uuid) {
  std::vector<::crucible::proto::RepoHeader> repo_headers;
  ::mongo::BSONObj query = BSON("repo_header.base_repo_uuid" << base_repo_uuid);
  auto cursor = connection_.query(table_, query);
  while (cursor->more()) {
    // Got a repo header back
    ::mongo::BSONObj bson = cursor->next();
    ::crucible::proto::RepoHeader repo_header;
    crucible_mapper_.repo_header_mapper.from_bson(bson, repo_header);
    repo_headers.emplace_back(std::move(repo_header));
  }
  return std::move(repo_headers);
}

bool CrucibleModel::SaveRepo(const ::crucible::proto::Repo repo) {
  connection_.update(
      table_, BSON("repo_header.repo_name" << repo.repo_header.repo_name
                                           << "repo_header.user_uuid"
                                           << repo.repo_header.user_uuid),
      crucible_mapper_.repo_mapper.to_bson(repo), true);

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
