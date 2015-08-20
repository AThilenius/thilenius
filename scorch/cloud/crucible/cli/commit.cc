// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/cli/commit.h"

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "scorch/cloud/crucible/crucible_repo.h"

using ::thilenius::scorch::cloud::crucible::CrucibleRepo;
using ::thilenius::scorch::cloud::crucible::RepoSyncStatus;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace cli {

int Commit(const std::string root_path, const std::vector<std::string>& args) {
  CrucibleRepo repo = CrucibleRepo::LoadFromDirectoryOrDie(root_path);
  switch (repo.GetSyncStatus()) {
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
  ::crucible::ChangeList change_list = repo.Commit();
  LOG(INFO) << "Commit compleated with commit Id: "
            << change_list.change_list_uuid;
  return 0;
}

}  // namespace cli
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
