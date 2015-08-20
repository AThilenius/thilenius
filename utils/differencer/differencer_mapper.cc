// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "utils/differencer/differencer_mapper.h"

#include "base/types.h"

namespace thilenius {
namespace utils {
namespace differencer {

::differencer::DiffType::type DifferencerMapper::IntToDiffType(
    int value) const {
  switch (value) {
    case 0: {
      return ::differencer::DiffType::INSERT;
    }
    case 1: {
      return ::differencer::DiffType::DELETE;
    }
    case 2: {
      return ::differencer::DiffType::EQUAL;
    }
  }
  return ::differencer::DiffType::UNKNOWN;
}

int DifferencerMapper::DiffTypeToInt(
    const ::differencer::DiffType::type& diff_type) const {
  switch (diff_type) {
    case ::differencer::DiffType::INSERT: {
      return 1;
    }
    case ::differencer::DiffType::DELETE: {
      return 2;
    }
    case ::differencer::DiffType::EQUAL: {
      return 3;
    }
    default: { return 0; }
  }
}

::differencer::Patch DifferencerMapper::JsonToPatch(
    const ::nlohmann::json& json) const {
  ::differencer::Patch patch;
  patch.from_span.from = json["from_span"]["from"].get<int>();
  patch.from_span.length = json["from_span"]["length"].get<int>();
  patch.to_span.from = json["to_span"]["from"].get<int>();
  patch.to_span.length = json["to_span"]["length"].get<int>();
  for (const auto& diff_json : json["diffs"]) {
    ::differencer::Diff diff;
    diff.diff_type = IntToDiffType(diff_json["diff_type"].get<int>());
    diff.text = diff_json["text"].get<std::string>();
    patch.diffs.emplace_back(std::move(diff));
  }
  return std::move(patch);
}

::nlohmann::json DifferencerMapper::PatchToJson(
    const ::differencer::Patch& patch) const {
  ::nlohmann::json json = {
      {"from_span",
       {
        {"from", patch.from_span.from}, {"length", patch.from_span.length}, }},
      {"to_span",
       {
        {"from", patch.to_span.from}, {"length", patch.to_span.length}, }},
      {"diffs", ::nlohmann::json::array()}
  };
  for (const auto& diff : patch.diffs) {
    ::nlohmann::json diff_json = {
        {"diff_type", DiffTypeToInt(diff.diff_type)},
        {"text", diff.text}
    };
    json["diffs"].push_back(std::move(diff_json));
  }
  return std::move(json);
}

}  // namespace differencer
}  // namespace utils
}  // namespace thilenius
