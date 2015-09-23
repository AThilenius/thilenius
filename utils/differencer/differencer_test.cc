// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gtest/gtest.h"
#include "base/log.h"
#include "utils/differencer/differencer.h"

using ::differencer::proto::Diff;
using ::differencer::proto::Patch;
using ::thilenius::utils::differencer::Differencer;

namespace thilenius {
namespace utils {
namespace differencer {
namespace {

class DifferencerTest : public testing::Test {
 protected:
  DifferencerTest()
      : first_string_("ABCDEFGHIJKLMENOPQRSTUVWXYZ"),
        second_string_("ABCDEFGHIJKLMENOPQRSTUVWXY"),
        third_string_("<new>ABCDEFGHIJKLMENOPQRSTUVWXY<also_new>") {}

  Differencer differencer_;
  std::string first_string_;
  std::string second_string_;
  std::string third_string_;
};

TEST_F(DifferencerTest, DiffTest) {
  std::vector<Patch> patches =
      differencer_.PatchesFromStrings(first_string_, second_string_);
  EXPECT_EQ(patches.size(), 1);
  EXPECT_EQ(patches[0].diffs.size(), 2);
}

TEST_F(DifferencerTest, NoDiff) {
  std::vector<Patch> patches =
      differencer_.PatchesFromStrings(first_string_, first_string_);
  EXPECT_EQ(patches.size(), 0);
}

TEST_F(DifferencerTest, LongDiffTest) {
  std::vector<Patch> patches =
      differencer_.PatchesFromStrings(second_string_, third_string_);
  EXPECT_EQ(patches.size(), 2);
  EXPECT_EQ(patches[0].diffs.size(), 2);
  EXPECT_EQ(patches[1].diffs.size(), 2);
}

TEST_F(DifferencerTest, PatchChain) {
  std::vector<Patch> patches =
      differencer_.PatchesFromStrings(first_string_, second_string_);
  std::vector<Patch> patches_two =
      differencer_.PatchesFromStrings(second_string_, third_string_);
  for (const auto& patch : patches_two) {
    patches.emplace_back(patch);
  }
  std::string output = differencer_.ApplyPatches(first_string_, patches);
  EXPECT_EQ(third_string_, output);
}

TEST_F(DifferencerTest, EmptyPatches) {
  std::vector<Patch> patches;
  EXPECT_EQ(first_string_, differencer_.ApplyPatches(first_string_, patches));
}

}  // namespace
}  // namespace differencer
}  // namespace utils
}  // namespace thilenius
