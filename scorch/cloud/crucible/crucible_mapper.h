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
  ::mongoxx::Mapper<::crucible::FileInfo> file_info_mapper;
  ::mongoxx::Mapper<::crucible::File> file_mapper;
  ::mongoxx::Mapper<::crucible::FileDelta> file_delta_mapper;
  ::mongoxx::Mapper<::crucible::ChangeList> change_list_mapper;
  ::mongoxx::Mapper<::crucible::RepoHeader> repo_header_mapper;
  ::mongoxx::Mapper<::crucible::Repo> repo_mapper;
  ::mongoxx::Mapper<::crucible::Snapshot> snapshot_mapper;
};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_
