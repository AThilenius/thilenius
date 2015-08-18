// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_model.h"

#include "base/log.h"
#include "base/types.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

bool CrucibleModel::FindRepoById(::crucible::Repo* repo,
                                 const std::string& repo_uuid) {
  return false;
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
    *repo = BsonObjToRepo(bson);
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
      ::mongo::fromjson(RepoToJson(repo).dump()), true);

  std::string last_error = connection_.getLastError();
  if (last_error != "") {
    LOG(WARNING) << "Got a MongoDB error: " << last_error;
    return false;
  }
  return true;
}

::crucible::Repo CrucibleModel::BsonObjToRepo(
    const ::mongo::BSONObj& bson_obj) {
  // TODO(athilenius): This is likley crazy slow, might want to hand write it
  ::nlohmann::json json = ::nlohmann::json::parse(bson_obj.jsonString());
  // Header
  ::crucible::RepoHeader repo_header;
  repo_header.repo_uuid = json["repo_uuid"].get<std::string>();
  if (json["base_repo_uuid"].get<std::string>() != "") {
    repo_header.base_repo_uuid = json["base_repo_uuid"].get<std::string>();
  }
  repo_header.user_uuid = json["user_uuid"].get<std::string>();
  repo_header.repo_name = json["repo_name"].get<std::string>();
  repo_header.creation_timestamp =
      std::stoll(json["creation_timestamp"]["$numberLong"].get<std::string>());
  // Repo
  ::crucible::Repo repo;
  repo.repo_header = std::move(repo_header);
  for (const auto& change_list_json : json["change_lists"]) {
    ::crucible::ChangeList change_list;
    change_list.change_list_uuid =
        change_list_json["change_list_uuid"].get<std::string>();
    change_list.user_uuid = change_list_json["user_uuid"].get<std::string>();
    change_list.timestamp = change_list_json["timestamp"].get<int64>();
    // TODO(athilenius): Finish files!
    repo.change_lists.emplace_back(std::move(change_list));
  }
  return std::move(repo);
}

::nlohmann::json CrucibleModel::RepoToJson(const ::crucible::Repo& repo) {
  ::nlohmann::json repo_json = {
      {"repo_uuid", repo.repo_header.repo_uuid},
      {"base_repo_uuid", repo.repo_header.base_repo_uuid},
      {"user_uuid", repo.repo_header.user_uuid},
      {"repo_name", repo.repo_header.repo_name},
      {"creation_timestamp", repo.repo_header.creation_timestamp},
      {"change_lists", ::nlohmann::json::array()}};
  for (const auto& change_list : repo.change_lists) {
    ::nlohmann::json cl_json = {
        {"change_list_uuid", change_list.change_list_uuid},
        {"user_uuid", change_list.user_uuid},
        {"timestamp", change_list.timestamp},
        {"added_files", ::nlohmann::json::array()},
        {"modified_files", ::nlohmann::json::array()},
        {"removed_files", ::nlohmann::json::array()}};
    repo_json["change_lists"].push_back(cl_json);
    // TODO(athilenius): Finish files!
  }
  return std::move(repo_json);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
