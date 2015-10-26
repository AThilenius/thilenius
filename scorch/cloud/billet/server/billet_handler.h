// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BILLET_SERVER_BILLET_HANDLER_H_
#define SCORCH_CLOUD_BILLET_SERVER_BILLET_HANDLER_H_

#include <unordered_map>

#include "base/mutex.h"
#include "cloud/sentinel/sentinel_types.h"
#include "scorch/cloud/billet/Billet.h"
#include "scorch/cloud/billet/billet_session.h"
#include "scorch/cloud/billet/billet_types.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {
namespace server {

class BilletHandler : virtual public ::billet::proto::BilletIf {
 public:
  BilletHandler();

  void SyncAndExec(::fiber::proto::Cord& _return,
                   const ::sentinel::proto::Token& token,
                   const ::crucible::proto::RepoHeader& repo_header,
                   const std::string& shell_command);

  void TerminateSession(const ::sentinel::proto::Token& token);

  void GetSessionStatus(::billet::proto::SessionStatus& _return,
                        const ::sentinel::proto::Token& token);

  void ClangFormat(std::string& _return, const std::string& source);

 private:
  std::mutex mutex_;
  std::unordered_map<std::string, BilletSessionPtr> sessions_;
};

}  // namespace server
}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BILLET_SERVER_BILLET_HANDLER_H_
