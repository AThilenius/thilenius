// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_mapper.h"

#include "base/log.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

CrucibleMapper::CrucibleMapper() {
  // File Info
  file_info_mapper.add_field("relative_path",
                             &::crucible::proto::FileInfo::relative_path);
  file_info_mapper.add_field("is_locked",
                             &::crucible::proto::FileInfo::is_locked);
  file_info_mapper.add_field("md5", &::crucible::proto::FileInfo::md5);
  file_info_mapper.add_field("modify_timestamp",
                             &::crucible::proto::FileInfo::modify_timestamp);

  // File
  file_mapper.add_field("file_info", &::crucible::proto::File::file_info,
                        file_info_mapper);
  file_mapper.add_field("file_type", &::crucible::proto::File::file_type);
  file_mapper.add_field("source", &::crucible::proto::File::source);

  // FileDelta
  file_delta_mapper.add_field(
      "file_info", &::crucible::proto::FileDelta::file_info, file_info_mapper);
  file_delta_mapper.add_field("patches", &::crucible::proto::FileDelta::patches,
                              differencer_mapper.patch_mapper);

  // ChangeList
  change_list_mapper.add_field(
      "change_list_uuid", &::crucible::proto::ChangeList::change_list_uuid);
  change_list_mapper.add_field("user_uuid",
                               &::crucible::proto::ChangeList::user_uuid);
  change_list_mapper.add_field("timestamp",
                               &::crucible::proto::ChangeList::timestamp);
  change_list_mapper.add_field(
      "added_files", &::crucible::proto::ChangeList::added_files, file_mapper);
  change_list_mapper.add_field("modified_files",
                               &::crucible::proto::ChangeList::modified_files,
                               file_delta_mapper);
  change_list_mapper.add_field("removed_files",
                               &::crucible::proto::ChangeList::removed_files,
                               file_info_mapper);

  // RepoHeader
  repo_header_mapper.add_field("repo_uuid",
                               &::crucible::proto::RepoHeader::repo_uuid);
  repo_header_mapper.add_field("base_repo_uuid",
                               &::crucible::proto::RepoHeader::base_repo_uuid);
  repo_header_mapper.add_field("user_uuid",
                               &::crucible::proto::RepoHeader::user_uuid);
  repo_header_mapper.add_field("repo_name",
                               &::crucible::proto::RepoHeader::repo_name);
  repo_header_mapper.add_field(
      "creation_timestamp", &::crucible::proto::RepoHeader::creation_timestamp);
  repo_header_mapper.add_field(
      "latest_change_list_uuid",
      &::crucible::proto::RepoHeader::latest_change_list_uuid);

  // Repo
  repo_mapper.add_field("repo_header", &::crucible::proto::Repo::repo_header,
                        repo_header_mapper);
  repo_mapper.add_field("change_lists", &::crucible::proto::Repo::change_lists,
                        change_list_mapper);

  // Snapshot
  snapshot_mapper.add_field(
      "repo_info", &::crucible::proto::Snapshot::repo_info, repo_header_mapper);
  snapshot_mapper.add_field("from_change_list",
                            &::crucible::proto::Snapshot::from_change_list,
                            change_list_mapper);
  snapshot_mapper.add_field("files", &::crucible::proto::Snapshot::files,
                            file_mapper);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
