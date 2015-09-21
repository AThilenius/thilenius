// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_REPO_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_REPO_H_

#include <string>
#include <map>

#include "cloud/sentinel/sentinel_client.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_mapper.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "utils/differencer/differencer.h"

using ::thilenius::utils::differencer::Differencer;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

enum class RepoSyncStatus { UNKNOWN, HEAD, BEHIND_HEAD };

class CrucibleRepo {
 public:
  CrucibleRepo();

  ValueOf<void> Connect(const std::string& crucible_ip, int crucible_port,
                        const std::string& crucible_route,
                        const std::string& project_path);

  ValueOf<void> SyncToHead();

  ValueOf<RepoSyncStatus> SyncStatus() const;

  // Gets all pending (added, modified, or removed) files, as they differ from
  // GetFileInfosForHeader. Results are filtered if it's a forked repo.
  ::crucible::proto::ChangeList GetPendingChangeList() const;

  // Reconstructs a repo as it existed at the given CL uuid.
  std::map<std::string, ::crucible::proto::File> ReconstructFilesForCL(
      const std::string& change_list_uuid) const;

  // Commits all pending changes
  ValueOf<::crucible::proto::ChangeList> Commit();

  // The token being used for sentinel authentication
  ::sentinel::proto::Token token_proto;
  std::string project_path;

 private:
  typedef std::shared_ptr<ThriftHttpClient<::crucible::proto::CrucibleClient>>
      ProtoCrucibleClientPtr;

  ::crucible::proto::File CrucibleFileFromDiskFile(
      const std::string& full_path, const std::string& relative_path) const;

  ::crucible::proto::FileDelta CrucibleFileDeltaFromDisk(
      const std::string& full_path, const std::string& relative_path) const;

  std::string ReconstructFileFromDiffs(const std::string& relative_path) const;

  ::crucible::proto::Repo repo_;
  ::sentinel::proto::Token token_;
  CrucibleMapper crucible_mapper_;
  Differencer differencer_;
  ProtoCrucibleClientPtr http_client_ptr_;
  bool connected_;
  int crucible_port_;
  std::string crucible_ip_;
  std::string crucible_route_;
  std::string project_path_;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_REPO_H_
