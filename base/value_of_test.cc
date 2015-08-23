// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <chrono>
#include <cstdio>
#include <fstream>
#include <thread>

#include "base/value_of.hh"
#include "base/gtest/gtest.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

TEST(ValueOfTest, PositiveCase) {
  ValueOf<int> is_good = 42;
  EXPECT_TRUE(is_good.IsValid());
  EXPECT_EQ(42, is_good.GetOrDie());
}

TEST(ValueOfTest, NegativeDeathTest) {
  ValueOf<int> is_bad = {42, "should show up as error"};
  EXPECT_FALSE(is_bad.IsValid());
  EXPECT_EQ("should show up as error", is_bad.GetError());
  EXPECT_EXIT(is_bad.GetOrDie(),
              ::testing::ExitedWithCode(EXIT_FAILURE),
              ".*should show up as error*");
}

}  // namespace
}  // namespace base
}  // namespace thilenius
