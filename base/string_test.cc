// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gtest/gtest.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

TEST(String, StrCatTest) {
  EXPECT_TRUE(StrCat("Hello, World") == "Hello, World");
  EXPECT_TRUE(StrCat("Hello,", " World") == "Hello, World");
  EXPECT_TRUE(StrCat("Hello,", " Wor", "ld") == "Hello, World");
  EXPECT_TRUE(StrCat("Hello,", "", " World", "") == "Hello, World");
}

TEST(String, EmptyTest) {
  EXPECT_TRUE(Empty(""));
  EXPECT_FALSE(Empty(" "));
  EXPECT_FALSE(Empty("Not Blank"));
}

TEST(String, BlankTest) {
  EXPECT_TRUE(Blank(""));
  EXPECT_TRUE(Blank(" "));
  EXPECT_TRUE(Blank("  "));
  EXPECT_FALSE(Blank("  .  "));
}

TEST(String, BeginsWithTest) {
  EXPECT_TRUE(BeginsWith("Hello, world", ""));
  EXPECT_TRUE(BeginsWith("Hello, world", "H"));
  EXPECT_TRUE(BeginsWith("Hello, world", "Hello"));
  EXPECT_TRUE(BeginsWith("Hello, world", "Hello, world"));
  EXPECT_FALSE(BeginsWith("Hello, world", " "));
  EXPECT_FALSE(BeginsWith("Hello, world", "ello"));
}

TEST(String, EndsWithTest) {
  EXPECT_TRUE(EndsWith("Hello, world", ""));
  EXPECT_TRUE(EndsWith("Hello, world", "d"));
  EXPECT_TRUE(EndsWith("Hello, world", "world"));
  EXPECT_TRUE(EndsWith("Hello, world", "Hello, world"));
  EXPECT_FALSE(EndsWith("Hello, world", " "));
  EXPECT_FALSE(EndsWith("Hello, world", "worl"));
}

}  // namespace
}  // namespace base
}  // namespace thilenius