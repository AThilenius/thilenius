// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BILLET_BILLET_SESSION_H_
#define SCORCH_CLOUD_BILLET_BILLET_SESSION_H_

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "base/mutex.h"
#include "base/process.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_types.h"
#include "scorch/cloud/billet/billet_types.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "utils/differencer/differencer.h"

using ::thilenius::base::ProcessPtr;
using ::thilenius::base::ValueOf;
using ::thilenius::utils::differencer::Differencer;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

class BilletSession;
typedef std::shared_ptr<BilletSession> BilletSessionPtr;

class BilletSession {
 public:
  BilletSession() = default;

  static BilletSessionPtr CreateSession(const std::string& fiber_ip,
                                        int fiber_port,
                                        const std::string& fiber_route);

  // Throws a ::billet::proto::SessionBusy directly
  ValueOf<::fiber::proto::Cord> HarnessRunAndDetatch(
      const sentinel::proto::Token& token, const std::string& mount_point,
      const std::string& shell_command);

  void TerminateSession();

  ::billet::proto::SessionStatus GetSessionStatus();

 private:
  // Sets up a local folder and clones a crucible repo into it, bringing that
  // repo to the given CL.
  ValueOf<void> BringRepoToCL(const std::string& repo_path,
                              const ::sentinel::proto::Token& token,
                              const ::crucible::proto::RepoHeader& repo_header);

  std::mutex mutex_;
  std::condition_variable cond_var_ GUARDED_BY(mutex_);
  ProcessPtr process_ GUARDED_BY(mutex_);
  // A copy of the last or active Fiber Cord
  ::fiber::proto::Cord cord_ GUARDED_BY(mutex_);
  std::string fiber_ip_;
  int fiber_port_;
  std::string fiber_route_;
};

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BILLET_BILLET_SESSION_H_
