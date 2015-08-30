// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "utils/differencer/differencer_mapper.h"

#include "base/types.h"

namespace thilenius {
namespace utils {
namespace differencer {

DifferencerMapper::DifferencerMapper() {
  // Diff
  diff_mapper.add_field("diff_type", &::differencer::Diff::diff_type);
  diff_mapper.add_field("text", &::differencer::Diff::text);

  // Span
  span_mapper.add_field("from", &::differencer::Span::from);
  span_mapper.add_field("length", &::differencer::Span::length);

  // Patch
  patch_mapper.add_field("from_span", &::differencer::Patch::from_span,
                         span_mapper);
  patch_mapper.add_field("to_span", &::differencer::Patch::to_span,
                         span_mapper);
  patch_mapper.add_field("diffs", &::differencer::Patch::diffs, diff_mapper);
}

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius
