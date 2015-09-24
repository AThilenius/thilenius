// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <gflags/gflags.h>
#include <string>
#include <vector>

#include "base/arguments.h"
#include "base/log.h"
#include "base/path.h"
#include "scorch/cloud/crucible/crucible_client.h"
#include "scorch/cloud/crucible/crucible_repo.h"

DEFINE_string(sentinel_ip, "localhost",
              "The ip enpoint of the Sentinel server");
DEFINE_int32(sentinel_port, 2100, "The port for the Sentinel server");
DEFINE_string(sentinel_route, "/", "The route of the Sentinel server");
DEFINE_string(crucible_ip, "localhost",
              "The ip enpoint of the Crucible server");
DEFINE_int32(crucible_port, 2200, "The port for the Crucible server");
DEFINE_string(crucible_route, "/", "The route of the Crucible server");

using ::thilenius::base::Arguments;
using ::thilenius::base::Path;
using ::thilenius::scorch::cloud::crucible::CrucibleClient;
using ::thilenius::scorch::cloud::crucible::CrucibleRepo;
using ::thilenius::scorch::cloud::crucible::RepoSyncStatus;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace cli {

int Commit(const std::string root_path, const std::vector<std::string>& args) {
  CrucibleClient crucible_client;
  crucible_client.Connect(FLAGS_crucible_ip, FLAGS_crucible_port,
                          FLAGS_crucible_route, FLAGS_sentinel_ip,
                          FLAGS_sentinel_port, FLAGS_sentinel_route);
  CrucibleRepo repo =
      crucible_client.LoadRepoFromDirectory(root_path).GetOrDie();
  switch (repo.SyncStatus().GetOrDie()) {
    case RepoSyncStatus::HEAD: {
      LOG(INFO) << "Status: HEAD";
      break;
    }
    case RepoSyncStatus::BEHIND_HEAD: {
      LOG(ERROR) << "Your repo is behind head, aka it is out of sync. "
                    "Redownload the latest repo snapshot";
      LOG(FATAL) << "Status: BEHIND_HEAD!";
      break;
    }
    default: {
      LOG(FATAL) << "Status: UNKNOWN";
      break;
    }
  }
  ::crucible::proto::ChangeList change_list = repo.Commit().GetOrDie();
  LOG(INFO) << "Commit compleated with commit Id: "
            << change_list.change_list_uuid;
  return 0;
}

int Create(const std::string root_path, const std::vector<std::string>& args) {
  if (args.size() < 1) {
    LOG(ERROR) << "Usage: crucible create [flags] <repo_name>";
    LOG(FATAL) << "Expected 1 argument, got " << args.size();
  }
  CrucibleClient crucible_client;
  crucible_client.Connect(FLAGS_crucible_ip, FLAGS_crucible_port,
                          FLAGS_crucible_route, FLAGS_sentinel_ip,
                          FLAGS_sentinel_port, FLAGS_sentinel_route);
  CrucibleRepo crucible_repo =
      crucible_client.CreateNewBaseRepoInDirectory(root_path, args[0])
          .GetOrDie();
  LOG(INFO) << "Created " << args[0];
  return 0;
}

int Clone(const std::string root_path, const std::vector<std::string>& args) {
  if (args.size() < 1) {
    LOG(ERROR) << "Usage: crucible clone [flags] <repo_name>";
    LOG(FATAL) << "Expected 1 argument, got " << args.size();
  }
  CrucibleClient crucible_client;
  crucible_client.Connect(FLAGS_crucible_ip, FLAGS_crucible_port,
                          FLAGS_crucible_route, FLAGS_sentinel_ip,
                          FLAGS_sentinel_port, FLAGS_sentinel_route);
  CrucibleRepo crucible_repo =
      crucible_client.CloneBaseRepoInDirectory(root_path, args[0]).GetOrDie();
  LOG(INFO) << "Cloned Base: " << args[0];
  return 0;
}

int ListRepos(const std::string root_path,
              const std::vector<std::string>& args) {
  CrucibleClient crucible_client;
  crucible_client.Connect(FLAGS_crucible_ip, FLAGS_crucible_port,
                          FLAGS_crucible_route, FLAGS_sentinel_ip,
                          FLAGS_sentinel_port, FLAGS_sentinel_route);
  std::vector<::crucible::proto::RepoHeader> repo_headers =
      crucible_client.GetRepoHeadersByUser().GetOrDie();
  for (const auto& repo_header : repo_headers) {
    LOG(INFO) << "  - " << repo_header.repo_name;
  }
  return 0;
}

int Status(const std::string root_path, const std::vector<std::string>& args) {
  CrucibleClient crucible_client;
  crucible_client.Connect(FLAGS_crucible_ip, FLAGS_crucible_port,
                          FLAGS_crucible_route, FLAGS_sentinel_ip,
                          FLAGS_sentinel_port, FLAGS_sentinel_route);
  CrucibleRepo repo =
      crucible_client.LoadRepoFromDirectory(root_path).GetOrDie();
  LOG(INFO) << "Loaded repo from disk at: " << root_path;
  switch (repo.SyncStatus().GetOrDie()) {
    case RepoSyncStatus::HEAD: {
      LOG(INFO) << "Status: HEAD";
      break;
    }
    case RepoSyncStatus::BEHIND_HEAD: {
      LOG(ERROR) << "Status: BEHIND_HEAD!";
      LOG(ERROR) << "Your repo is behind head, aka it is out of sync. "
                    "Redownload a repo snapshot";
      break;
    }
    default: {
      LOG(ERROR) << "Status: UNKNOWN";
      break;
    }
  }
  CrucibleMapper mapper_;
  ::crucible::proto::ChangeList pending = repo.GetPendingChangeList();
  if (pending.added_files.size() != 0) {
    LOG(INFO) << "Pending Adds:";
    for (const auto& file : pending.added_files) {
      LOG(INFO) << "  + " << file.file_info.relative_path;
    }
  }
  if (pending.modified_files.size() != 0) {
    LOG(INFO) << "Pending Modifications:";
    for (const auto& file_delta : pending.modified_files) {
      LOG(INFO) << "  ~ " << file_delta.file_info.relative_path;
    }
  }
  if (pending.removed_files.size() != 0) {
    LOG(INFO) << "Pending Removals:";
    for (const auto& file_info : pending.removed_files) {
      LOG(INFO) << "  - " << file_info.relative_path;
    }
  }
  if (pending.added_files.size() == 0 && pending.modified_files.size() == 0 &&
      pending.removed_files.size() == 0) {
    LOG(INFO) << "No modified files. All files up to date.";
  }
  return 0;
}

}  // namespace cli
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc < 2) {
    LOG(FATAL) << "Usage: crucible <command> <command_args>";
  }
  std::string root_path = Path::CurrentPath();
  std::string command = argv[1];
  std::vector<std::string> args = Arguments::ToVector(argc - 2, &argv[2]);
  if (command == "commit") {
    return ::thilenius::scorch::cloud::crucible::cli::Commit(root_path, args);
  } else if (command == "clone") {
    return ::thilenius::scorch::cloud::crucible::cli::Clone(root_path, args);
  } else if (command == "list_repos") {
    return ::thilenius::scorch::cloud::crucible::cli::ListRepos(root_path,
                                                                args);
  } else if (command == "create") {
    return ::thilenius::scorch::cloud::crucible::cli::Create(root_path, args);
  } else if (command == "status") {
    return ::thilenius::scorch::cloud::crucible::cli::Status(root_path, args);
  } else {
    LOG(FATAL) << "Unknown CLI command " << command;
  }
}
