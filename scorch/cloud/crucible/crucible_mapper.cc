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
                             &::crucible::FileInfo::relative_path);
  file_info_mapper.add_field("md5", &::crucible::FileInfo::md5);
  file_info_mapper.add_field("modify_timestamp",
                             &::crucible::FileInfo::modify_timestamp);

  // File
  file_mapper.add_field("file_info", &::crucible::File::file_info,
                        file_info_mapper);
  file_mapper.add_field("file_type", &::crucible::File::file_type);
  file_mapper.add_field("source", &::crucible::File::source);

  // FileDelta
  file_delta_mapper.add_field("file_info", &::crucible::FileDelta::file_info,
                              file_info_mapper);
  file_delta_mapper.add_field("patches", &::crucible::FileDelta::patches,
                              differencer_mapper.patch_mapper);

  // ChangeList
  change_list_mapper.add_field("change_list_uuid",
                               &::crucible::ChangeList::change_list_uuid);
  change_list_mapper.add_field("user_uuid", &::crucible::ChangeList::user_uuid);
  change_list_mapper.add_field("timestamp", &::crucible::ChangeList::timestamp);
  change_list_mapper.add_field(
      "added_files", &::crucible::ChangeList::added_files, file_mapper);
  change_list_mapper.add_field("modified_files",
                               &::crucible::ChangeList::modified_files,
                               file_delta_mapper);
  change_list_mapper.add_field("removed_files",
                               &::crucible::ChangeList::removed_files,
                               file_info_mapper);

  // RepoHeader
  repo_header_mapper.add_field("repo_uuid", &::crucible::RepoHeader::repo_uuid);
  repo_header_mapper.add_field("base_repo_uuid",
                               &::crucible::RepoHeader::base_repo_uuid);
  repo_header_mapper.add_field("user_uuid", &::crucible::RepoHeader::user_uuid);
  repo_header_mapper.add_field("repo_name", &::crucible::RepoHeader::repo_name);
  repo_header_mapper.add_field("creation_timestamp",
                               &::crucible::RepoHeader::creation_timestamp);
  repo_header_mapper.add_field(
      "latest_change_list_uuid",
      &::crucible::RepoHeader::latest_change_list_uuid);

  // Repo
  repo_mapper.add_field("repo_header", &::crucible::Repo::repo_header,
                        repo_header_mapper);
  repo_mapper.add_field("change_lists", &::crucible::Repo::change_lists,
                        change_list_mapper);

  // Snapshot
  snapshot_mapper.add_field("repo_info", &::crucible::Snapshot::repo_info,
                            repo_header_mapper);
  snapshot_mapper.add_field("from_change_list",
                            &::crucible::Snapshot::from_change_list,
                            change_list_mapper);
  snapshot_mapper.add_field("files", &::crucible::Snapshot::files, file_mapper);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
