// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_SERVER_CRUCIBLE_HANDLER_H_
#define SCORCH_CLOUD_CRUCIBLE_SERVER_CRUCIBLE_HANDLER_H_

#include "cloud/sentinel/sentinel_types.h"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_mapper.h"
#include "scorch/cloud/crucible/crucible_model.h"
#include "third_party/mongo/client/dbclient.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {
namespace server {

class CrucibleHandler : virtual public ::crucible::CrucibleIf {
 public:
  CrucibleHandler();

  void CreateNewRepo(::crucible::Repo& _return,
                     const ::sentinel::Token& user_stoken,
                     const std::string& repo_name);

  void CreateForkedRepo(::crucible::Repo& _return,
                        const ::sentinel::Token& user_stoken,
                        const std::string& base_repo_uuid);

  void GetRepoHeadersByUser(std::vector<::crucible::RepoHeader>& _return,
                            const ::sentinel::Token& user_stoken);

  void GetRepoById(::crucible::Repo& _return,
                   const ::sentinel::Token& user_stoken,
                   const std::string& repo_uuid);

  void GetRepoHeaderById(::crucible::RepoHeader& _return,
                         const ::sentinel::Token& user_stoken,
                         const std::string& repo_uuid);

  void CommitAndDownstream(::crucible::ChangeList& _return,
                           const ::sentinel::Token& user_stoken,
                           const ::crucible::RepoHeader& repo_header,
                           const ::crucible::ChangeList& change_list);

 private:
  void AuthenticateOrThrow(const ::sentinel::Token& token) const;

  ::mongo::DBClientConnection mongo_connection_;
  CrucibleModel model_;
  CrucibleMapper crucible_mapper_;
};

}  // namespace server
}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_SERVER_CRUCIBLE_HANDLER_H_
