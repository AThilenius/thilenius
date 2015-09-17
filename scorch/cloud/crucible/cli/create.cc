// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/cli/create.h"

#include "base/log.h"
#include "scorch/cloud/crucible/crucible_repo.h"

using ::thilenius::scorch::cloud::crucible::CrucibleRepo;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace cli {

int Create(const std::string root_path, const std::vector<std::string>& args) {
  if (args.size() < 1) {
    LOG(ERROR) << "Usage: crucible create [flags] <repo_name>";
    LOG(FATAL) << "Expected 1 argument, got " << args.size();
  }
  CrucibleRepo crucible_repo =
      CrucibleRepo::CreateNewInDirectoryOrDie(root_path, args[0]);
  LOG(INFO) << "Created a new repo with ID: " << crucible_repo.GetRepoId();
  return 0;
}

}  // namespace cli
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
