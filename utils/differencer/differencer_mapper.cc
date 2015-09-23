// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "utils/differencer/differencer_mapper.h"

#include "base/types.h"

namespace thilenius {
namespace utils {
namespace differencer {

DifferencerMapper::DifferencerMapper() {
  // Diff
  diff_mapper.add_field("diff_type", &::differencer::proto::Diff::diff_type);
  diff_mapper.add_field("text", &::differencer::proto::Diff::text);

  // Span
  span_mapper.add_field("from", &::differencer::proto::Span::from);
  span_mapper.add_field("length", &::differencer::proto::Span::length);

  // Patch
  patch_mapper.add_field("from_span", &::differencer::proto::Patch::from_span,
                         span_mapper);
  patch_mapper.add_field("to_span", &::differencer::proto::Patch::to_span,
                         span_mapper);
  patch_mapper.add_field("diffs", &::differencer::proto::Patch::diffs, diff_mapper);
}

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius
