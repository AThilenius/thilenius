// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/auto_update.h"

#include "base/directory.h"
#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/http.h"
#include "base/json.h"
#include "base/log.h"
#include "base/path.h"
#include "base/process.h"
#include "base/string.h"
#include "base/value_of.hh"

DEFINE_bool(no_update, false, "Skips updating the assembly");

namespace thilenius {
namespace base {

void AutoUpdate::UpdateAtCurrentExecutable(const std::string& update_json_url) {
  // JSON of format:
  // {
  //   latest: {
  //     version_string: string
  //     files: [
  //       {
  //         name: string
  //         url: string
  //         target_path: string
  //       }, ...
  //   }
  // }
  if (FLAGS_no_update) {
    return;
  }
  std::string exe_path = Path::ExecutablePath();
  std::string exe_parent_path = Path::ParentPath(exe_path);
  ValueOf<std::string> json_value = Http::GetContent(update_json_url);
  if (!json_value.IsValid()) {
    LOG(WARNING) << "Failed to update, could not get update JSON at "
                 << update_json_url;
    return;
  }
  ::nlohmann::json json = ::nlohmann::json::parse(json_value.GetOrDie());
  std::string this_version = gflags::VersionString();
  if (!json["latest"].is_object()) {
    LOG(WARNING) << "Failed to parse update JSON";
    return;
  }
  std::string latest_version =
      json["latest"]["version_string"].get<std::string>();
  if (latest_version != this_version) {
    std::vector<::nlohmann::json> files_json = json["latest"]["files"];
    LOG(INFO) << "Updating from version " << this_version << " to version "
              << latest_version;
    LOG(INFO) << "Downloading " << files_json.size() << " files:";
    for (const auto& file_json : files_json) {
      std::string name = file_json["name"].get<std::string>();
      std::string url = file_json["url"].get<std::string>();
      std::string target_path = file_json["target_path"].get<std::string>();
      LOG(INFO) << "Downloading: " << Path::Combine(exe_parent_path, name)
                << "...";
      std::string error = Http::DownloadToFile(
          url, Path::Combine(exe_parent_path, StrCat(target_path, "_tmp")));
      if (!String::Blank(error)) {
        LOG(ERROR) << "Failed to download " << url << " to "
                   << Path::Combine(exe_parent_path, name);
        return;
      }
    }
    LOG(INFO) << "Installing...";
    for (const auto& file_json : files_json) {
      std::string name = file_json["name"].get<std::string>();
      std::string url = file_json["url"].get<std::string>();
      std::string target_path = file_json["target_path"].get<std::string>();
      if (!File::Rename(
              Path::Combine(exe_parent_path, target_path),
              Path::Combine(exe_parent_path, StrCat(target_path, "_swp")))) {
        LOG(ERROR) << "Failed to rename "
                   << Path::Combine(exe_parent_path, target_path);
      }
      if (!File::Rename(
              Path::Combine(exe_parent_path, StrCat(target_path, "_tmp")),
              Path::Combine(exe_parent_path, target_path))) {
        LOG(ERROR) << "Failed to rename "
                   << Path::Combine(exe_parent_path, target_path);
      }
      File::Remove(Path::Combine(exe_parent_path, StrCat(target_path, "_swp")));
    }
    LOG(INFO) << "done. Up to date!";
    LOG(INFO) << "Trying to relaunch...";
    // Need to mark it executable first
    Process::ExecuteCommandSync("chmod", {"+x", exe_path}, true);
    std::vector<std::string> args = ::gflags::GetArgvs();
    args.push_back("--no_update");
    int exit_code = Process::ExecuteCommandSync(exe_path, args, false);
    exit(exit_code);
  }
  // Clean up any missed files
  for (const auto& file :
       Directory::GetChildrenFilesRecursive(exe_parent_path)) {
    if (String::EndsWith(file, "_swp") || String::EndsWith(file, "_tmp")) {
      LOG(WARNING) << "Cleaning up a stray update file: " << file;
      File::Remove(file);
    }
  }
}

}  // namespace base
}  // namespace thilenius
