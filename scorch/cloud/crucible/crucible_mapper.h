// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_
#define SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_

#include <vector>

#include "base/json.h"
#include "base/types.h"
#include "scorch/cloud/crucible/crucible_constants.h"
#include "scorch/cloud/crucible/crucible_types.h"
#include "utils/differencer/differencer_mapper.h"

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

class CrucibleMapper {
 public:
  // From JSON
  int64 JsonToTimestamp(const ::nlohmann::json timestamp_json) const;
  ::crucible::ChangeList JsonToChangeList(
      const ::nlohmann::json change_list_json) const;

  ::crucible::Repo JsonToRepo(const ::nlohmann::json json) const;

  // To Json
  ::nlohmann::json ChangeListToJson(
      const ::crucible::ChangeList change_list) const;

  ::nlohmann::json RepoToJson(const ::crucible::Repo& repo) const;

 private:
  ::thilenius::utils::differencer::DifferencerMapper difference_mapper_;

};

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_CLOUD_CRUCIBLE_CRUCIBLE_MAPPER_H_
