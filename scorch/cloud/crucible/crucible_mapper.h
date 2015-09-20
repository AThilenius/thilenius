// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_

#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "third_party/mongoxx/mongoxx.hh"
#include "utils/differencer/differencer_mapper.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

class CrucibleMapper {
 public:
  CrucibleMapper();

  ::thilenius::utils::differencer::DifferencerMapper differencer_mapper;
  ::mongoxx::Mapper<::crucible::proto::FileInfo> file_info_mapper;
  ::mongoxx::Mapper<::crucible::proto::File> file_mapper;
  ::mongoxx::Mapper<::crucible::proto::FileDelta> file_delta_mapper;
  ::mongoxx::Mapper<::crucible::proto::ChangeList> change_list_mapper;
  ::mongoxx::Mapper<::crucible::proto::RepoHeader> repo_header_mapper;
  ::mongoxx::Mapper<::crucible::proto::Repo> repo_mapper;
  ::mongoxx::Mapper<::crucible::proto::RepoState> repo_state_mapper;
  ::mongoxx::Mapper<::crucible::proto::Snapshot> snapshot_mapper;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_
