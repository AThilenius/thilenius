// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_BILLET_BILLET_SESSION_H_
#define SCORCH_CLOUD_BILLET_BILLET_SESSION_H_

#include <string>
#include <vector>

#include "base/process.h"
#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_types.h"
#include "scorch/cloud/billet/billet_types.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "utils/differencer/differencer.h"

using ::thilenius::base::Process;
using ::thilenius::base::ValueOf;
using ::thilenius::utils::differencer::Differencer;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace billet {

class BilletSession {
 public:
  // Creates a local session (run unprotected in the same process space as the
  // server)
  static BilletSession CreateLocalSession(
      const ::billet::proto::Session& billet_session_proto);

  ValueOf<void> ExecuteCMakeRepo(
      const ::crucible::proto::RepoHeader& repo_header_proto,
      const std::vector<::crucible::proto::ChangeList>& staged_change_list,
      const std::vector<std::string>& application_args);

  ValueOf<::billet::proto::ApplicationOutput> GetOutputTillLine(int line);

  ::billet::proto::Session billet_session_proto;

 private:
  // Sets up a local folder and clones a crucible repo into it, bringing that
  // repo to the given CL and applying the given staged change lists.
  ValueOf<void> BringRepoToCL(
      const std::string& repo_path,
      const ::sentinel::proto::Token& token,
      const ::crucible::proto::RepoHeader& repo_header,
      const std::vector<::crucible::proto::ChangeList>& staged_change_list);

  bool is_running;
  Process process_;
  Differencer differencer_;
};

}  // namespace billet
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_BILLET_BILLET_SESSION_H_
