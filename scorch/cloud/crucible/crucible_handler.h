// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_HANDLER_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_HANDLER_H_

#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_model.h"
#include "third_party/mongo/client/dbclient.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

class CrucibleHandler : virtual public ::crucible::CrucibleIf {
 public:
  CrucibleHandler();

  void CreateNewRepo(::crucible::Repo& _return, const std::string& user_uuid,
                     const std::string& repo_name);

  void CreateForkedRepo(::crucible::Repo& _return, const std::string& user_uuid,
                        const std::string& base_repo_uuid);

  void GetRepoHeadersByUser(std::vector<::crucible::RepoHeader>& _return,
                            const std::string& user_uuid);

  void GetRepoById(::crucible::Repo& _return, const std::string& repo_uuid);

  void CommitAndDownstream(::crucible::ChangeList& _return,
                           const std::string& repo_uuid,
                           const ::crucible::ChangeList& change_list);

 private:
  ::mongo::DBClientConnection mongo_connection_;
  CrucibleModel model_;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_HANDLER_H_
