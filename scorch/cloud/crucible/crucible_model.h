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

// STRUCTURE -
// {
//   repo_uuid,
//   base_repo_uuid,
//   user_uuid,
//   repo_name,
//   creation_timestamp
//   change_lists: [
//     {
//       change_list_uuid,
//       user_uuid,
//       timestamp,
//       added_files: [ File.. ],
//       modified_files: [ FileDelta... ],
//       removed_files: [ FileInfo... ]
//     }, { ...
//   ]
// }, { ...

class CrucibleModel {
 public:
  CrucibleModel(::mongo::DBClientConnection& connection,
                const std::string table)
      : connection_(connection), table_(table) {}

  // Querying
  bool FindRepoById(::crucible::proto::Repo* repo, const std::string& repo_uuid);

  bool FindRepoByUserIdAndRepoName(::crucible::proto::Repo* repo,
                                   const std::string& user_uuid,
                                   const std::string& repo_name);

  std::vector<::crucible::proto::Repo> FindReposByUserId(const std::string& user_uuid);

  std::vector<::crucible::proto::Repo> FindReposByRepoBaseId(
      const std::string& base_repo_uuid);

  // Saving
  bool SaveRepo(const ::crucible::proto::Repo repo);

 private:
  CrucibleMapper crucible_mapper_;
  ::mongo::DBClientConnection& connection_;
  const std::string table_;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MODEL_H_
