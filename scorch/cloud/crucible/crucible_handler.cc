// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_handler.h"

#include "base/gflags/gflags.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"

DEFINE_string(source_path, "/data/static_assert/crucible",
              "The root path of the saved files");

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

void CrucibleHandler::CloneCrucibleBaseRepoFromGit(
    ::crucible::RepoBase& _return, const std::string& git_url,
    const std::string& git_path) {}

void CrucibleHandler::GetRepoInfoFromBase(::crucible::RepoInfo& _return,
                                          const std::string& user_uuid,
                                          const std::string& base_uuid) {}

void CrucibleHandler::CommitSnapshot(::crucible::RepoInfo& _return,
                                     const std::string& repo_uuid,
                                     const ::crucible::Snapshot& snapshot) {}

void CrucibleHandler::GetSnapshot(::crucible::Snapshot& _return,
                                  const std::string& snapshot_uuid) {}

void CrucibleHandler::GetSnapshotAsTar(::crucible::ExternalFile& _return,
                                       const std::string& snapshot_id) {}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

int main() {}
