// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MODEL_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MODEL_H_

#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_mapper.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "third_party/mongo/client/dbclient.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

class CrucibleModel {
 public:
  CrucibleModel();

  // Querying
  bool FindRepoById(::crucible::proto::Repo* repo,
                    const std::string& repo_uuid);

  bool FindRepoByRepoName(::crucible::proto::Repo* repo,
                          const std::string& repo_name);

  std::vector<::crucible::proto::RepoHeader> FindRepoHeadersByUserId(
      const std::string& user_uuid);

  std::vector<::crucible::proto::RepoHeader> FindAllBaseRepos();

  // Saving
  bool SaveRepo(const ::crucible::proto::Repo repo);

 private:
  CrucibleMapper crucible_mapper_;
  ::mongo::DBClientConnection connection_;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MODEL_H_
