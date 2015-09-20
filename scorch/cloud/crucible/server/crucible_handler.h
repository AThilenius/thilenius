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

class CrucibleHandler : virtual public ::crucible::proto::CrucibleIf {
 public:
  CrucibleHandler();

  void CreateNewRepo(::crucible::proto::Repo& _return,
                     const ::sentinel::proto::Token& user_stoken,
                     const std::string& repo_name);

  void CreateForkedRepo(::crucible::proto::Repo& _return,
                        const ::sentinel::proto::Token& user_stoken,
                        const std::string& base_repo_uuid);

  void GetRepoHeadersByUser(std::vector<::crucible::proto::RepoHeader>& _return,
                            const ::sentinel::proto::Token& user_stoken);

  void GetRepoById(::crucible::proto::Repo& _return,
                   const ::sentinel::proto::Token& user_stoken,
                   const std::string& repo_uuid);

  void GetRepoHeaderById(::crucible::proto::RepoHeader& _return,
                         const ::sentinel::proto::Token& user_stoken,
                         const std::string& repo_uuid);

  void CommitAndDownstream(::crucible::proto::ChangeList& _return,
                           const ::sentinel::proto::Token& user_stoken,
                           const ::crucible::proto::RepoHeader& repo_header,
                           const ::crucible::proto::ChangeList& change_list);

 private:
  void AuthenticateOrThrow(const ::sentinel::proto::Token& token) const;

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
