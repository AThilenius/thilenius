// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_mapper.h"

#include "base/log.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

int64 CrucibleMapper::JsonToTimestamp(
    const ::nlohmann::json timestamp_json) const {
  if (timestamp_json.is_number()) {
    return timestamp_json.get<int64>();
  } else {
    return std::stoll(timestamp_json["$numberLong"].get<std::string>());
  }
}

::crucible::ChangeList CrucibleMapper::JsonToChangeList(
    const ::nlohmann::json change_list_json) const {
  ::crucible::ChangeList change_list;
  change_list.change_list_uuid =
      change_list_json["change_list_uuid"].get<std::string>();
  change_list.user_uuid = change_list_json["user_uuid"].get<std::string>();
  change_list.timestamp = JsonToTimestamp(change_list_json["timestamp"]);
  // Added files
  for (const auto& file_json : change_list_json["added_files"]) {
    ::crucible::File file;
    file.file_info.realative_path =
        file_json["file_info"]["realative_path"].get<std::string>();
    file.file_info.md5 = file_json["file_info"]["md5"].get<std::string>();
    file.file_info.modify_timestamp = JsonToTimestamp(file_json["file_info"]["modify_timestamp"]);
    switch (file_json["file_type"].get<int>()) {
      case 1: {
        file.file_type = ::crucible::FileType::TEXT;
        file.text_source = file_json["text_source"].get<std::string>();
      }
      case 2: {
        file.file_type = ::crucible::FileType::BINARY;
        file.binary_source = file_json["binary_source"].get<std::string>();
      }
      case 3: {
        file.file_type = ::crucible::FileType::URL;
        file.url_source = file_json["url_source"].get<std::string>();
      }
    }
    change_list.added_files.emplace_back(std::move(file));
  }
  // Modified files
  for (const auto& file_delta_json : change_list_json["modified_files"]) {
    ::crucible::FileDelta file_delta;
    file_delta.file_info.realative_path =
        file_delta_json["file_info"]["realative_path"].get<std::string>();
    file_delta.file_info.md5 =
        file_delta_json["file_info"]["md5"].get<std::string>();
    file_delta.file_info.modify_timestamp = JsonToTimestamp(file_delta_json["file_info"]["modify_timestamp"]);
    for (const auto& patch_json : file_delta_json["patches"]) {
      file_delta.patches.emplace_back(
          difference_mapper_.JsonToPatch(patch_json));
    }
    change_list.modified_files.emplace_back(std::move(file_delta));
  }
  // Deleted files
  for (const auto& file_info_json : change_list_json["removed_files"]) {
    ::crucible::FileInfo file_info;
    file_info.realative_path =
        file_info_json["realative_path"].get<std::string>();
    file_info.md5 = file_info_json["md5"].get<std::string>();
    file_info.modify_timestamp = JsonToTimestamp(file_info_json["modify_timestamp"]);
    change_list.removed_files.emplace_back(std::move(file_info));
  }
  return std::move(change_list);
}

::crucible::Repo CrucibleMapper::JsonToRepo(const ::nlohmann::json json) const {
  ::crucible::RepoHeader repo_header;
  repo_header.repo_uuid = json["repo_uuid"].get<std::string>();
  if (json["base_repo_uuid"].get<std::string>() != "") {
    repo_header.base_repo_uuid = json["base_repo_uuid"].get<std::string>();
  }
  repo_header.user_uuid = json["user_uuid"].get<std::string>();
  repo_header.repo_name = json["repo_name"].get<std::string>();
  repo_header.creation_timestamp = JsonToTimestamp(json["creation_timestamp"]);
  repo_header.latest_change_list_uuid =
      json["latest_change_list_uuid"].get<std::string>();
  // Repo
  ::crucible::Repo repo;
  repo.repo_header = std::move(repo_header);
  for (const auto& change_list_json : json["change_lists"]) {
    repo.change_lists.emplace_back(JsonToChangeList(change_list_json));
  }
  return std::move(repo);
}

::nlohmann::json CrucibleMapper::ChangeListToJson(
    const ::crucible::ChangeList change_list) const {
  ::nlohmann::json cl_json = {
      {"change_list_uuid", change_list.change_list_uuid},
      {"user_uuid", change_list.user_uuid},
      {"timestamp", change_list.timestamp},
      {"added_files", ::nlohmann::json::array()},
      {"modified_files", ::nlohmann::json::array()},
      {"removed_files", ::nlohmann::json::array()}};
  // Added files
  for (const auto& file : change_list.added_files) {
    ::nlohmann::json file_json = {
        {"file_info",
         {{"realative_path", file.file_info.realative_path},
          {"md5", file.file_info.md5},
          {"modify_timestamp", file.file_info.modify_timestamp}}}};
    switch (file.file_type) {
      case ::crucible::FileType::TEXT: {
        file_json["text_source"] = file.text_source;
        break;
      }
      case ::crucible::FileType::BINARY: {
        file_json["binary_source"] = file.binary_source;
        break;
      }
      case ::crucible::FileType::URL: {
        file_json["url_source"] = file.url_source;
        break;
      }
      default: {
        LOG(WARNING) << "Unknown Crucible FileType while converting Crucible "
                        "Repo to JSON";
      }
    }
    cl_json["added_files"].push_back(std::move(file_json));
  }
  // Modified files
  for (const auto& file_delta : change_list.modified_files) {
    ::nlohmann::json file_delta_json = {
        {
         "file_info",
         {{"realative_path", file_delta.file_info.realative_path},
          {"md5", file_delta.file_info.md5},
          {"modify_timestamp", file_delta.file_info.modify_timestamp}},
        },
        {"patches", ::nlohmann::json::array()}};
    for (const auto& patch : file_delta.patches) {
      file_delta_json["patches"].push_back(
          difference_mapper_.PatchToJson(patch));
    }
    cl_json["modified_files"].push_back(std::move(file_delta_json));
  }
  // Removed files
  for (const auto& file_info : change_list.removed_files) {
    ::nlohmann::json file_info_json = {
        {"realative_path", file_info.realative_path},
        {"md5", file_info.md5},
        {"modify_timestamp", file_info.modify_timestamp}};
    cl_json["removed_files"].push_back(std::move(file_info_json));
  }
  return std::move(cl_json);
}

::nlohmann::json CrucibleMapper::RepoToJson(
    const ::crucible::Repo& repo) const {
  ::nlohmann::json repo_json = {
      {"repo_uuid", repo.repo_header.repo_uuid},
      {"base_repo_uuid", repo.repo_header.base_repo_uuid},
      {"user_uuid", repo.repo_header.user_uuid},
      {"repo_name", repo.repo_header.repo_name},
      {"creation_timestamp", repo.repo_header.creation_timestamp},
      {"latest_change_list_uuid", repo.repo_header.latest_change_list_uuid},
      {"change_lists", ::nlohmann::json::array()}};
  for (const auto& change_list : repo.change_lists) {
    repo_json["change_lists"].push_back(ChangeListToJson(change_list));
  }
  return std::move(repo_json);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
