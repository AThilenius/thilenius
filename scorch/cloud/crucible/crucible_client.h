// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_CLIENT_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_CLIENT_H_

#include <string>

#include "cloud/sentinel/sentinel_client.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/Crucible.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_mapper.h"
#include "scorch/cloud/crucible/crucible_repo.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "utils/differencer/differencer.h"

using ::thilenius::cloud::sentinel::SentinelClient;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

class CrucibleClient {
 public:
  CrucibleClient();

  // Connects to the specified Crucible instance.
  ValueOf<void> Connect(const std::string& crucible_ip, int crucible_port,
                        const std::string& crucible_route,
                        const std::string& sentinel_ip, int sentinel_port,
                        const std::string& sentinel_route);

  ValueOf<CrucibleRepo> CreateNewBaseRepoInDirectory(
      const std::string& path, const std::string& repo_name);

  // Clones and existing repo (base or forked) into the given directory
  ValueOf<CrucibleRepo> CloneRepoInDirectory(
      const std::string& path, const std::string& repo_uuid);

  ValueOf<CrucibleRepo> CloneBaseRepoInDirectory(
      const std::string& path, const std::string& base_repo_name);

  ValueOf<CrucibleRepo> LoadRepoFromDirectory(const std::string& path);

  ValueOf<std::vector<::crucible::proto::RepoHeader>> GetRepoHeadersByUser();

 private:
  typedef std::shared_ptr<ThriftHttpClient<::crucible::proto::CrucibleClient>>
      ProtoCrucibleClientPtr;

  ValueOf<::sentinel::proto::Token> LoginAndAuthroSecondary(
      const std::string& path);

  CrucibleMapper crucible_mapper_;
  ProtoCrucibleClientPtr http_client_ptr_;
  SentinelClient sentinel_client_;
  bool connected_;
  int crucible_port_;
  int sentinel_port_;
  std::string crucible_ip_;
  std::string crucible_route_;
  std::string sentinel_ip_;
  std::string sentinel_route_;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_CLIENT_H_
