// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_
#define UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_

#include "third_party/mongoxx/mongoxx.hh"
#include "utils/differencer/differencer_constants.h"
#include "utils/differencer/differencer_types.h"

namespace thilenius {
namespace utils {
namespace differencer {

class DifferencerMapper {
 public:
   DifferencerMapper();

   ::mongoxx::Mapper<::differencer::Diff> diff_mapper;
   ::mongoxx::Mapper<::differencer::Span> span_mapper;
   ::mongoxx::Mapper<::differencer::Patch> patch_mapper;
};

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius

#endif  // UTILS_DIFFERENCER_DIFFERENCER_MAPPER_H_
