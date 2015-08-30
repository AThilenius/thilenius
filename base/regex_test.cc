// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/regex.h"
#include "base/gtest/gtest.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

TEST(RegexTest, FindZeroMatches) {
  EXPECT_FALSE(Regex::FindFirstMatch("somestr", "dne").IsValid());
}

TEST(RegexTest, FindOneMatches) {
  EXPECT_TRUE(Regex::FindFirstMatch("somestr", "me").IsValid());
  EXPECT_GT(Regex::FindAllMatches("somestr", "me").size(), 0);
}

TEST(RegexTest, FindManyMatches) {
  std::vector<std::string> matches =
      Regex::FindAllMatches("a1,a2a3 a4a", "a(\\d)");
  EXPECT_EQ(4, matches.size());
  EXPECT_EQ("1", matches[0]);
  EXPECT_EQ("2", matches[1]);
  EXPECT_EQ("3", matches[2]);
  EXPECT_EQ("4", matches[3]);
}

}  // namespace
}  // namespace base
}  // namespace thilenius
