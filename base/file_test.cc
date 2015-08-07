// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/file.h"
#include "base/gtest/gtest.h"

namespace thilenius {
namespace base {
namespace {

TEST(Existance, ShouldExist) {
  EXPECT_TRUE(File::Exists("/root/thilenius/base/file_test.cc"))
      << "File::Exists returned false for a file that that exists.";
}

TEST(Existance, RelativePaths) {
  // Remember that the executable is at /root/thilenius_bin/base/
  EXPECT_TRUE(File::Exists("Makefile"))
      << "File::Exists returned false for a relative file that that exists.";
}

TEST(Existance, ShouldNotExist) {
  EXPECT_FALSE(File::Exists("this_file_does_not_exist"))
      << "File::Exists returned true for a file that does not exist.";
}

TEST(ReadContents, DeathTest) {
  EXPECT_EXIT(File::ReadContentsOrDie("file_does_not_exist"),
              ::testing::ExitedWithCode(EXIT_FAILURE),
              ".*Failed to find file.*");
}

TEST(ReadContents, ReadThisTest) {
  EXPECT_TRUE(File::ReadContentsOrDie("/root/thilenius/base/file_test.cc")
                  .length() != 0)
      << "File::ReadContentsOrDie returned a zero length string for an "
         "existing text file.";
}

}  // namespace
}  // namespace base
}  // namespace thilenius
