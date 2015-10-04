// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BILLET_SERVER_BILLET_HANDLER_H_
#define SCORCH_CLOUD_BILLET_SERVER_BILLET_HANDLER_H_

#include <unordered_map>

#include "cloud/sentinel/sentinel_client.h"
#include "cloud/sentinel/sentinel_types.h"
#include "scorch/cloud/billet/Billet.h"
#include "scorch/cloud/billet/billet_session.h"
#include "scorch/cloud/billet/billet_types.h"

using ::thilenius::cloud::sentinel::SentinelClient;
using ::thilenius::scorch::cloud::billet::BilletSession;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {
namespace server {

class BilletHandler : virtual public ::billet::proto::BilletIf {
 public:
  BilletHandler();

  void CreateSession(::billet::proto::Session& _return,
                     const ::sentinel::proto::Token& sentinel_token);

  void ExecuteCMakeRepo(
      const ::billet::proto::Session& session,
      const ::crucible::proto::RepoHeader& repo_header,
      const std::vector< ::crucible::proto::ChangeList>& staged_change_lists,
      const std::vector<std::string>& application_args);

  void QueryOutputAfterLine(::billet::proto::ApplicationOutput& _return,
                            const ::billet::proto::Session& session,
                            const int32_t line);

 private:
  void ThrowOpFailure(const std::string& message);

  void AuthenticateOrThrow(const ::sentinel::proto::Token& token);

  SentinelClient sentinel_client_;
  std::unordered_map<std::string, BilletSession> sessions_;
};

}  // namespace server
}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BILLET_SERVER_BILLET_HANDLER_H_
