// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/cli/status.h"

#include "base/log.h"
#include "scorch/cloud/crucible/crucible_mapper.h"
#include "scorch/cloud/crucible/crucible_repo.h"

using ::thilenius::scorch::cloud::crucible::CrucibleMapper;
using ::thilenius::scorch::cloud::crucible::CrucibleRepo;
using ::thilenius::scorch::cloud::crucible::RepoSyncStatus;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace cli {

int Status(const std::string root_path, const std::vector<std::string>& args) {
  CrucibleRepo repo = CrucibleRepo::LoadFromDirectoryOrDie(root_path);
  switch (repo.GetSyncStatus()) {
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
  ::crucible::ChangeList pending = repo.GetPending();
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
