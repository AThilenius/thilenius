// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_REPO_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_REPO_H_

#include <string>
#include <map>

#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

enum class RepoSyncStatus { UNKNOWN, HEAD, BEHIND_HEAD };

class CrucibleRepo {
 public:
  static CrucibleRepo CreateNewInDirectoryOrDie(const std::string& path,
                                                const std::string user_name,
                                                const std::string repo_name);

  static CrucibleRepo LoadFromDirectoryOrDie(const std::string& path);

  std::string GetRepoId() const;

  RepoSyncStatus GetSyncStatus() const;

  // Gets all pending (added, modified, or removed) files, as they differ from
  // GetFileInfosForHeader
  ::crucible::ChangeList GetPending() const;

  // Runs though all diffs and computes what files should be active in head,
  // returned as a map of realative file names to FileInfo
  std::map<std::string, ::crucible::FileInfo> GetFileInfosForHead() const;

  // Reconstructs a single file (by relative path) from diffs
  std::string ReconstructFileFromDiffs(const std::string& relative_path) const;

  ::crucible::ChangeList Commit();

 private:
  ::crucible::File CrucibleFileFromDiskFile(
      const std::string& full_path, const std::string& relative_path) const;
  ::crucible::FileDelta CrucibleFileDeltaFromDisk(
      const std::string& full_path, const std::string& relative_path) const;

  std::string path_;
  ::crucible::Repo repo_;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_REPO_H_
