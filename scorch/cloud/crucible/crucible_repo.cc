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
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_mapper.h"
#include "utils/differencer/differencer.h"

DEFINE_string(crucible_endpoint, "localhost",
              "The endpoint for the Crucible server.");
DEFINE_string(crucible_endpoint_route, "/",
              "The http route for the Crucible server.");
DEFINE_int32(crucible_endpoint_port, 80, "The port for the Crucible server.");
DEFINE_string(crucible_dir_name, ".crucible",
              "The name of the dir crucible uses to store data.");
DEFINE_string(crucible_repo_file_cache_name, "crucible_repo.json",
              "The name of the file within crucible_dir_name that crucible "
              "uses to store the serialized repo.");

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::String;
using ::thilenius::base::Time;

namespace {
::thilenius::scorch::cloud::crucible::CrucibleMapper crucible_mapper_;
::thilenius::cloud::utils::ThriftHttpClient<::crucible::CrucibleClient>
    http_client_(FLAGS_crucible_endpoint, FLAGS_crucible_endpoint_port,
                 FLAGS_crucible_endpoint_route);
::thilenius::utils::differencer::Differencer differencer_;
}  // namespace

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

CrucibleRepo CrucibleRepo::CreateNewInDirectoryOrDie(
    const std::string& path, const std::string& user_name,
    const std::string& repo_name) {
  std::string crucible_dir_path = Path::Combine(path, FLAGS_crucible_dir_name);
  std::string crucible_repo_json_path =
      Path::Combine(crucible_dir_path, FLAGS_crucible_repo_file_cache_name);
  if (!Directory::Exists(path)) {
    // Create the directory
    LOG(WARNING) << "Directory " << path << " does not exist. Creating it.";
    Directory::Create(path);
  } else {
    // Make sure there isn't already a .crucilble folder
    if (Directory::Exists(crucible_dir_path) &&
        Directory::GetChildren(crucible_dir_path).size() != 0) {
      LOG(FATAL) << "Cannot create a new repo at " << path
                 << ", a repo already exists there";
    }
  }
  Directory::Create(crucible_dir_path);
  CrucibleRepo repo;
  repo.path_ = path;
  try {
    http_client_.ConnectOrDie()->CreateNewRepo(repo.repo_, user_name,
                                               repo_name);
  } catch (::crucible::OperationFailure op_failure) {
    LOG(FATAL) << "Crucible remote exception: " << op_failure.user_message;
  } catch (...) {
    LOG(FATAL) << "In CreateNewRepo, Crucible server threw an unhandled "
                  "exception.";
  }
  // Also write it out to file
  std::string json = crucible_mapper_.repo_mapper.to_json(repo.repo_);
  File::WriteToFile(crucible_repo_json_path, json);
  return std::move(repo);
}

CrucibleRepo CrucibleRepo::LoadFromDirectoryOrDie(const std::string& path) {
  std::string crucible_dir_path = Path::Combine(path, FLAGS_crucible_dir_name);
  std::string crucible_repo_json_path =
      Path::Combine(crucible_dir_path, FLAGS_crucible_repo_file_cache_name);
  if (!Directory::Exists(path)) {
    LOG(FATAL) << "Crucible failed to find the directory " << path;
  }
  if (!File::Exists(crucible_repo_json_path)) {
    LOG(FATAL) << "Crucible failed to find a repo in " << path;
  }
  std::string json = File::ReadContentsOrDie(crucible_repo_json_path);
  CrucibleRepo crucible_repo;
  crucible_repo.path_ = path;
  crucible_repo.repo_ =
      crucible_mapper_.repo_mapper.from_bson(::mongo::fromjson(json));
  return std::move(crucible_repo);
}

std::string CrucibleRepo::GetRepoId() const {
  return repo_.repo_header.repo_uuid;
}

RepoSyncStatus CrucibleRepo::GetSyncStatus() const {
  ::crucible::RepoHeader repo_header;
  try {
    http_client_.ConnectOrDie()->GetRepoHeaderById(repo_header,
                                                   repo_.repo_header.repo_uuid);
  } catch (::crucible::OperationFailure op_failure) {
    LOG(FATAL) << "Crucible remote exception: " << op_failure.user_message;
  } catch (...) {
    LOG(FATAL) << "In GetRepoHeaderById, Crucible server threw an unhandled "
                  "exception.";
  }
  if (repo_header.latest_change_list_uuid !=
      repo_.repo_header.latest_change_list_uuid) {
    return RepoSyncStatus::BEHIND_HEAD;
  }
  return RepoSyncStatus::HEAD;
}

::crucible::ChangeList CrucibleRepo::GetPending() const {
  std::string crucible_dir_path = Path::Combine(path_, FLAGS_crucible_dir_name);
  std::map<std::string, ::crucible::FileInfo> header_infos =
      GetFileInfosForHead();
  ::crucible::ChangeList change_list;
  change_list.user_uuid = repo_.repo_header.user_uuid;
  change_list.timestamp = std::to_string(Time::EpochMilliseconds());
  // First find all files (Glob them)
  std::set<std::string> all_files;
  for (const auto& full_path : Directory::GetChildrenFiles(path_)) {
    all_files.insert(full_path);
  }
  // Then sub dirs, but filter out .crucible
  for (const auto& full_path : Directory::GetChildrenDirectories(path_)) {
    if (!String::EndsWith(Path::WithoutEdgeSlashes(full_path),
                          FLAGS_crucible_dir_name)) {
      for (const auto& full_sub_path :
           Directory::GetChildrenFilesRecursive(full_path)) {
        all_files.insert(full_sub_path);
      }
    }
  }
  // Added / Modified files
  for (const auto& full_path : all_files) {
    std::string file = Path::RelativePath(full_path, path_);
    auto iter = header_infos.find(file);
    if (iter == header_infos.end()) {
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
  // Removed files
  std::set<std::string> relative_all_files;
  for (const auto& full_path : all_files) {
    relative_all_files.insert(Path::RelativePath(full_path, path_));
  }
  for (const auto& header_info : header_infos) {
    if (relative_all_files.find(header_info.first) ==
        relative_all_files.end()) {
      ::crucible::FileInfo file_info;
      file_info.relative_path = header_info.first;
      file_info.modify_timestamp = std::to_string(Time::EpochMilliseconds());
      change_list.removed_files.emplace_back(std::move(file_info));
    }
  }
  return std::move(change_list);
}

std::map<std::string, ::crucible::FileInfo> CrucibleRepo::GetFileInfosForHead()
    const {
  std::map<std::string, ::crucible::FileInfo> active_files;
  for (const auto& change_list : repo_.change_lists) {
    for (const auto& file : change_list.added_files) {
      active_files[file.file_info.relative_path] = file.file_info;
    }
    for (const auto& file_delta : change_list.modified_files) {
      active_files[file_delta.file_info.relative_path] = file_delta.file_info;
    }
    for (const auto& file_info : change_list.removed_files) {
      auto iter = active_files.find(file_info.relative_path);
      active_files.erase(iter);
    }
  }
  return std::move(active_files);
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

::crucible::File CrucibleRepo::CrucibleFileFromDiskFile(
    const std::string& full_path, const std::string& relative_path) const {
  ::crucible::File file;
  file.file_info.relative_path = relative_path;
  file.file_info.md5 = File::MD5OrDie(full_path);
  file.file_info.modify_timestamp =
      std::to_string(File::LastWriteTime(full_path));
  file.file_type = constants_.FILE_TYPE_TEXT;
  file.source = File::ReadContentsOrDie(full_path);
  return std::move(file);
}

::crucible::FileDelta CrucibleRepo::CrucibleFileDeltaFromDisk(
    const std::string& full_path, const std::string& relative_path) const {
  ::crucible::FileDelta file_delta;
  file_delta.file_info.relative_path = relative_path;
  file_delta.file_info.md5 = File::MD5OrDie(full_path);
  file_delta.file_info.modify_timestamp =
      std::to_string(File::LastWriteTime(full_path));
  std::string old_content = ReconstructFileFromDiffs(relative_path);
  file_delta.patches = differencer_.PatchesFromStrings(
      old_content, File::ReadContentsOrDie(full_path));
  return std::move(file_delta);
}

::crucible::ChangeList CrucibleRepo::Commit() {
  std::string crucible_dir_path = Path::Combine(path_, FLAGS_crucible_dir_name);
  std::string crucible_repo_json_path =
      Path::Combine(crucible_dir_path, FLAGS_crucible_repo_file_cache_name);
  ::crucible::ChangeList old_change_list = GetPending();
  ::crucible::ChangeList new_change_list;
  try {
    http_client_.ConnectOrDie()->CommitAndDownstream(
        new_change_list, repo_.repo_header, old_change_list);
  } catch (::crucible::OperationFailure op_failure) {
    LOG(FATAL) << "Crucible remote exception: " << op_failure.user_message;
  } catch (...) {
    LOG(FATAL) << "In CommitAndDownstream, Crucible server threw an unhandled "
                  "exception.";
  }
  repo_.repo_header.latest_change_list_uuid = new_change_list.change_list_uuid;
  repo_.change_lists.push_back(new_change_list);
  // Save it to disk
  std::string json = crucible_mapper_.repo_mapper.to_json(repo_);
  File::WriteToFile(crucible_repo_json_path, json);
  return std::move(new_change_list);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
