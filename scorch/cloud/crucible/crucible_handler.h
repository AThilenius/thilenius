// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_HANDLER_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_HANDLER_H_

#include "scorch/cloud/crucible/Crucible.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

class CrucibleHandler : virtual public ::crucible::CrucibleIf {
 public:
  void CloneCrucibleBaseRepoFromGit(::crucible::RepoBase& _return,
                                    const std::string& git_url,
                                    const std::string& git_path);

  void GetRepoInfoFromBase(::crucible::RepoInfo& _return,
                           const std::string& user_uuid,
                           const std::string& base_uuid);

  void CommitSnapshot(::crucible::RepoInfo& _return,
                      const std::string& repo_uuid,
                      const ::crucible::Snapshot& snapshot);

  void GetSnapshot(::crucible::Snapshot& _return,
                   const std::string& snapshot_uuid);

  void GetSnapshotAsTar(::crucible::ExternalFile& _return,
                        const std::string& snapshot_id);
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_HANDLER_H_
