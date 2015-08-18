// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "utils/differencer/differencer.h"

#include "base/log.h"
#include "utils/differencer/diff_match_patch.h"

namespace thilenius {
namespace utils {
namespace differencer {

std::vector<::differencer::Patch> Differencer::PatchesFromStrings(
    const std::string& from, const std::string& to) const {
  std::vector<::differencer::Patch> patches;
  typedef diff_match_patch<std::string> DiffMatchPatch;
  DiffMatchPatch dmp;
  std::list<DiffMatchPatch::Diff> dmp_diffs = dmp.diff_main(from, to);
  dmp.diff_cleanupSemantic(dmp_diffs);
  std::list<DiffMatchPatch::Patch> dmp_patches = dmp.patch_make(dmp_diffs);
  for (const auto& dmp_patch : dmp_patches) {
    ::differencer::Patch patch;
    patch.from_span.from = dmp_patch.start1;
    patch.from_span.length = dmp_patch.length1;
    patch.to_span.from = dmp_patch.start2;
    patch.to_span.length = dmp_patch.length2;
    for (const auto& dmp_diff : dmp_patch.diffs) {
      switch (dmp_diff.operation) {
        case DiffMatchPatch::INSERT: {
          ::differencer::Diff thrift_diff;
          thrift_diff.diff_type = ::differencer::DiffType::INSERT;
          thrift_diff.text = dmp_diff.text;
          patch.diffs.emplace_back(std::move(thrift_diff));
          break;
        }
        case DiffMatchPatch::DELETE: {
          ::differencer::Diff thrift_diff;
          thrift_diff.diff_type = ::differencer::DiffType::DELETE;
          thrift_diff.text = dmp_diff.text;
          patch.diffs.emplace_back(std::move(thrift_diff));
          break;
        }
        case DiffMatchPatch::EQUAL: {
          ::differencer::Diff thrift_diff;
          thrift_diff.diff_type = ::differencer::DiffType::EQUAL;
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
    const std::vector<::differencer::Patch>& patches) const {
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
      switch (diff.diff_type) {
        case ::differencer::DiffType::INSERT: {
          dmp_diff.operation = DiffMatchPatch::INSERT;
          break;
        }
        case ::differencer::DiffType::DELETE: {
          dmp_diff.operation = DiffMatchPatch::DELETE;
          break;
        }
        case ::differencer::DiffType::EQUAL: {
          dmp_diff.operation = DiffMatchPatch::EQUAL;
          break;
        }
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
    const ::differencer::Patch& patch) const {
  LOG(INFO) << "From [" << patch.from_span.from << ", "
            << (patch.from_span.from + patch.from_span.length) << "]";
  LOG(INFO) << "  To [" << patch.to_span.from << ", "
            << (patch.to_span.from + patch.to_span.length) << "]";
  for (const auto& diff : patch.diffs) {
    std::string type_string;
    switch (diff.diff_type) {
      case ::differencer::DiffType::INSERT: {
        type_string = "Insert";
        break;
      }
      case ::differencer::DiffType::DELETE: {
        type_string = "Delete";
        break;
      }
      case ::differencer::DiffType::EQUAL: {
        type_string = "Equal";
        break;
      }
    }
    LOG(INFO) << "  - " << type_string << ": " << diff.text;
  }
}

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius
