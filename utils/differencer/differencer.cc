// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "utils/differencer/differencer.h"

#include "base/log.h"
#include "utils/differencer/diff_match_patch.h"

namespace thilenius {
namespace utils {
namespace differencer {

std::vector<::differencer::proto::Patch> Differencer::PatchesFromStrings(
    const std::string& from, const std::string& to) const {
  ::differencer::proto::differencerConstants constants;
  std::vector<::differencer::proto::Patch> patches;
  typedef diff_match_patch<std::string> DiffMatchPatch;
  DiffMatchPatch dmp;
  std::list<DiffMatchPatch::Diff> dmp_diffs = dmp.diff_main(from, to);
  dmp.diff_cleanupSemantic(dmp_diffs);
  std::list<DiffMatchPatch::Patch> dmp_patches = dmp.patch_make(dmp_diffs);
  for (const auto& dmp_patch : dmp_patches) {
    ::differencer::proto::Patch patch;
    patch.from_span.from = dmp_patch.start1;
    patch.from_span.length = dmp_patch.length1;
    patch.to_span.from = dmp_patch.start2;
    patch.to_span.length = dmp_patch.length2;
    for (const auto& dmp_diff : dmp_patch.diffs) {
      switch (dmp_diff.operation) {
        case DiffMatchPatch::INSERT: {
          ::differencer::proto::Diff thrift_diff;
          thrift_diff.diff_type = constants.DIFF_TYPE_INSERT;
          thrift_diff.text = dmp_diff.text;
          patch.diffs.emplace_back(std::move(thrift_diff));
          break;
        }
        case DiffMatchPatch::DELETE: {
          ::differencer::proto::Diff thrift_diff;
          thrift_diff.diff_type = constants.DIFF_TYPE_DELETE;
          thrift_diff.text = dmp_diff.text;
          patch.diffs.emplace_back(std::move(thrift_diff));
          break;
        }
        case DiffMatchPatch::EQUAL: {
          ::differencer::proto::Diff thrift_diff;
          thrift_diff.diff_type = constants.DIFF_TYPE_EQUAL;
          thrift_diff.text = dmp_diff.text;
          patch.diffs.emplace_back(std::move(thrift_diff));
          break;
        }
      }
    }
    patches.emplace_back(std::move(patch));
  }
  return std::move(patches);
}

std::string Differencer::ApplyPatches(
    const std::string& original,
    const std::vector<::differencer::proto::Patch>& patches) const {
  ::differencer::proto::differencerConstants constants;
  // Convert back into DiffMatchPatch patches
  typedef diff_match_patch<std::string> DiffMatchPatch;
  DiffMatchPatch dmp;
  std::list<DiffMatchPatch::Patch> dmp_patches;
  for (const auto& patch : patches) {
    DiffMatchPatch::Patch dmp_patch;
    dmp_patch.start1 = patch.from_span.from;
    dmp_patch.length1 = patch.from_span.length;
    dmp_patch.start2 = patch.to_span.from;
    dmp_patch.length2 = patch.to_span.length;
    for (const auto& diff : patch.diffs) {
      DiffMatchPatch::Diff dmp_diff;
      if (diff.diff_type == constants.DIFF_TYPE_INSERT) {
        dmp_diff.operation = DiffMatchPatch::INSERT;
      } else if (diff.diff_type == constants.DIFF_TYPE_DELETE) {
        dmp_diff.operation = DiffMatchPatch::DELETE;
      } else if (diff.diff_type == constants.DIFF_TYPE_EQUAL) {
        dmp_diff.operation = DiffMatchPatch::EQUAL;
      }
      dmp_diff.text = diff.text;
      dmp_patch.diffs.emplace_back(std::move(dmp_diff));
    }
    dmp_patches.emplace_back(std::move(dmp_patch));
  }
  std::pair<std::string, std::vector<bool>> patch_apply_output =
      dmp.patch_apply(dmp_patches, original);
  return std::move(std::get<0>(patch_apply_output));
}

void Differencer::PrintPatchDebugString(
    const ::differencer::proto::Patch& patch) const {
  ::differencer::proto::differencerConstants constants;
  LOG(INFO) << "From [" << patch.from_span.from << ", "
            << (patch.from_span.from + patch.from_span.length) << "]";
  LOG(INFO) << "  To [" << patch.to_span.from << ", "
            << (patch.to_span.from + patch.to_span.length) << "]";
  for (const auto& diff : patch.diffs) {
    std::string type_string;
    if (diff.diff_type == constants.DIFF_TYPE_INSERT) {
        type_string = "Insert";
    } else if (diff.diff_type == constants.DIFF_TYPE_DELETE) {
        type_string = "Delete";
    } else if (diff.diff_type == constants.DIFF_TYPE_EQUAL) {
        type_string = "Equal";
    } else {
      type_string = "Unknown";
    }
    LOG(INFO) << "  - " << type_string << ": " << diff.text;
  }
}

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius
