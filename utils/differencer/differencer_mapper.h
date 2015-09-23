// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_
#define UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_

#include "third_party/mongoxx/mongoxx.hh"
#include "utils/differencer/gen-cpp2/differencer_constants.h"
#include "utils/differencer/gen-cpp2/differencer_types.h"

namespace thilenius {
namespace utils {
namespace differencer {

class DifferencerMapper {
 public:
   DifferencerMapper();

   ::mongoxx::Mapper<::differencer::proto::Diff> diff_mapper;
   ::mongoxx::Mapper<::differencer::proto::Span> span_mapper;
   ::mongoxx::Mapper<::differencer::proto::Patch> patch_mapper;
};

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius

#endif  // UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_
