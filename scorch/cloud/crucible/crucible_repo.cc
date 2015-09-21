// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_repo.h"

#include <set>

#include "base/directory.h"
#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/log.h"
#include "base/path.h"
#include "base/string.h"
#include "base/time.h"

DECLARE_string(crucible_dir_name);
DECLARE_string(crucible_repo_file_cache_name);

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::String;
using ::thilenius::base::Time;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

CrucibleRepo::CrucibleRepo() : connected_(false) {}

ValueOf<void> CrucibleRepo::Connect(const std::string& crucible_ip,
                                    int crucible_port,
                                    const std::string& crucible_route,
                                    const std::string& project_path) {
  if (connected_) {
    return {"Already connected"};
  }
  crucible_ip_ = crucible_ip;
  crucible_port_ = crucible_port;
  crucible_route_ = crucible_route;
  project_path_ = project_path;
  // Connect crucible
  http_client_ptr_ = ProtoCrucibleClientPtr(
      new ThriftHttpClient<::crucible::proto::CrucibleClient>(
          crucible_ip, crucible_port, crucible_route));
  auto connection = http_client_ptr_->Connect();
  if (!connection.IsValid()) {
    return {connection.GetError()};
  }
  connected_ = true;
  return {};
}

ValueOf<void> CrucibleRepo::SyncToHead() { return {}; }

ValueOf<RepoSyncStatus> CrucibleRepo::SyncStatus() const {
  if (!connected_) {
    return {RepoSyncStatus::UNKNOWN, "Must be connected before using a repo"};
  }
  ::crucible::proto::RepoHeader repo_header;
  try {
    http_client_ptr_->ConnectOrDie()->GetRepoHeaderById(
        repo_header, token_, repo_.repo_header.repo_uuid);
  } catch (::crucible::proto::OperationFailure op_failure) {
    return {RepoSyncStatus::UNKNOWN,
            StrCat("Crucible remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {
        RepoSyncStatus::UNKNOWN,
        "In GetRepoHeaderById, Crucible server threw an unhandled exception."};
  }
  if (repo_header.latest_change_list_uuid !=
      repo_.repo_header.latest_change_list_uuid) {
    return RepoSyncStatus::BEHIND_HEAD;
  }
  return RepoSyncStatus::HEAD;
}

::crucible::proto::ChangeList CrucibleRepo::GetPendingChangeList() const {
  std::string crucible_dir_path =
      Path::Combine(project_path_, FLAGS_crucible_dir_name);
  // First, reconstruct what head looks like
  std::map<std::string, ::crucible::proto::FileInfo> head_file_infos;
  for (const auto& change_list : repo_.change_lists) {
    for (const auto& file : change_list.added_files) {
      head_file_infos[file.file_info.relative_path] = file.file_info;
    }
    for (const auto& file_delta : change_list.modified_files) {
      head_file_infos[file_delta.file_info.relative_path] =
          file_delta.file_info;
    }
    for (const auto& file_info : change_list.removed_files) {
      auto iter = head_file_infos.find(file_info.relative_path);
      head_file_infos.erase(iter);
    }
  }
  ::crucible::proto::ChangeList change_list;
  change_list.user_uuid = repo_.repo_header.user_uuid;
  change_list.timestamp = std::to_string(Time::EpochMilliseconds());
  // First find all files recursive (Glob them) without the cruible_repo.json
  // If this is a forked repo, then just consider files already in the repo,
  // that aren't frozen
  bool is_forked_repo = !String::Empty(repo_.repo_header.base_repo_uuid);
  std::set<std::string> all_files;
  if (is_forked_repo) {
    for (const auto& head_file_info : head_file_infos) {
      if (!head_file_info.second.is_locked) {
        all_files.insert(Path::Combine(project_path_, head_file_info.first));
      }
    }
  } else {
    for (const auto& full_path :
         Directory::GetChildrenFilesRecursive(project_path_)) {
      if (String::EndsWith(Path::WithoutEdgeSlashes(full_path),
                           FLAGS_crucible_repo_file_cache_name)) {
        continue;
      }
      all_files.insert(full_path);
    }
  }
  // Added / Modified files
  for (const auto& full_path : all_files) {
    std::string file = Path::RelativePath(full_path, project_path_);
    auto iter = head_file_infos.find(file);
    if (iter == head_file_infos.end()) {
      // It's a new file, add it to the change list
      change_list.added_files.emplace_back(
          CrucibleFileFromDiskFile(full_path, file));
    } else {
      // File might have been modified (It's in both lists), check it's MD5
      std::string md5 = File::MD5OrDie(full_path);
      if (md5 != iter->second.md5) {
        // Modified file
        change_list.modified_files.emplace_back(
            CrucibleFileDeltaFromDisk(full_path, file));
      }
    }
  }
  // Convert all files over to relative for the rest of this
  std::set<std::string> relative_all_files;
  for (const auto& full_path : all_files) {
    relative_all_files.insert(Path::RelativePath(full_path, project_path_));
  }
  // Removed files
  for (const auto& header_info : head_file_infos) {
    if (relative_all_files.find(header_info.first) ==
        relative_all_files.end()) {
      ::crucible::proto::FileInfo file_info;
      file_info.relative_path = header_info.first;
      file_info.modify_timestamp = std::to_string(Time::EpochMilliseconds());
      change_list.removed_files.emplace_back(std::move(file_info));
    }
  }
  return std::move(change_list);
}

std::map<std::string, ::crucible::proto::File>
CrucibleRepo::ReconstructFilesForCL(const std::string& change_list_uuid) const {
  std::map<std::string, ::crucible::proto::File> active_files;
  for (const auto& change_list : repo_.change_lists) {
    for (const auto& file : change_list.added_files) {
      active_files[file.file_info.relative_path] = file;
    }
    for (const auto& file_delta : change_list.modified_files) {
      ::crucible::proto::File& file =
          active_files[file_delta.file_info.relative_path];
      file.source = differencer_.ApplyPatches(file.source, file_delta.patches);
    }
    for (const auto& file_info : change_list.removed_files) {
      auto iter = active_files.find(file_info.relative_path);
      active_files.erase(iter);
    }
  }
  return std::move(active_files);
}

ValueOf<::crucible::proto::ChangeList> CrucibleRepo::Commit() {
  if (!connected_) {
    return {::crucible::proto::ChangeList(),
            "Must be connected before using a repo"};
  }
  std::string crucible_dir_path =
      Path::Combine(project_path_, FLAGS_crucible_dir_name);
  std::string crucible_repo_json_path =
      Path::Combine(crucible_dir_path, FLAGS_crucible_repo_file_cache_name);
  ::crucible::proto::ChangeList old_change_list = GetPendingChangeList();
  ::crucible::proto::ChangeList new_change_list;
  try {
    http_client_ptr_->ConnectOrDie()->CommitAndDownstream(
        new_change_list, token_, repo_.repo_header, old_change_list);
  } catch (::crucible::proto::OperationFailure op_failure) {
    return {::crucible::proto::ChangeList(),
            StrCat("Crucible remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {::crucible::proto::ChangeList(),
            "In Commit, Crucible server threw an unhandled exception."};
  }
  repo_.repo_header.latest_change_list_uuid =
      new_change_list.change_list_uuid;
  repo_.change_lists.push_back(new_change_list);
  // Save it to disk
  std::string json = crucible_mapper_.repo_mapper.to_json(repo_);
  File::WriteToFile(crucible_repo_json_path, json);
  return std::move(new_change_list);
}

::crucible::proto::File CrucibleRepo::CrucibleFileFromDiskFile(
    const std::string& full_path, const std::string& relative_path) const {
  ::crucible::proto::File file;
  file.file_info.relative_path = relative_path;
  file.file_info.md5 = File::MD5OrDie(full_path);
  file.file_info.is_locked = false;
  file.file_info.modify_timestamp =
      std::to_string(File::LastWriteTime(full_path));
  file.file_type = ::crucible::proto::g_crucible_constants.FILE_TYPE_TEXT;
  file.source = File::ReadContentsOrDie(full_path);
  return std::move(file);
}

::crucible::proto::FileDelta CrucibleRepo::CrucibleFileDeltaFromDisk(
    const std::string& full_path, const std::string& relative_path) const {
  ::crucible::proto::FileDelta file_delta;
  file_delta.file_info.relative_path = relative_path;
  file_delta.file_info.md5 = File::MD5OrDie(full_path);
  file_delta.file_info.is_locked = false;
  file_delta.file_info.modify_timestamp =
      std::to_string(File::LastWriteTime(full_path));
  std::string old_content = ReconstructFileFromDiffs(relative_path);
  file_delta.patches = differencer_.PatchesFromStrings(
      old_content, File::ReadContentsOrDie(full_path));
  return std::move(file_delta);
}

std::string CrucibleRepo::ReconstructFileFromDiffs(
    const std::string& relative_path) const {
  std::string contents;
  for (const auto& change_list : repo_.change_lists) {
    // Added in this CL
    for (const auto& file : change_list.added_files) {
      if (file.file_info.relative_path == relative_path) {
        contents = file.source;
        break;
      }
    }
    // Modified in this CL
    for (const auto& file_delta : change_list.modified_files) {
      if (file_delta.file_info.relative_path == relative_path) {
        contents = differencer_.ApplyPatches(contents, file_delta.patches);
      }
    }
    // Removed in this CL
    for (const auto& file_info : change_list.removed_files) {
      if (file_info.relative_path == relative_path) {
        contents = "";
      }
    }
  }
  return std::move(contents);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
