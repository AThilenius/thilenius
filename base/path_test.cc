// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <fstream>

#include "base/path.h"
#include "base/gtest/gtest.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

class PathTest : public testing::Test {
 protected:
  PathTest()
      : full_dir_("/root/thilenius_bin/base"),
        file_cont_("Doesn't matter"),
        full_path_("/root/thilenius_bin/base/path_test_file.txt"),
        rel_path_("path_test_file.txt") {}

  virtual void SetUp() {
    std::remove(rel_path_.c_str());
    std::ofstream test_file(rel_path_);
    test_file << file_cont_;
  }

  virtual void TearDown() { std::remove(rel_path_.c_str()); }

  std::string full_dir_;
  std::string file_cont_;
  std::string full_path_;
  std::string rel_path_;
};

TEST_F(PathTest, AbsoluteTest) {
  EXPECT_EQ(Path::Absolute(rel_path_), full_path_);
  EXPECT_EQ(Path::Absolute(full_path_), full_path_);
}

TEST_F(PathTest, Combine) {
  EXPECT_EQ(Path::Combine("/starts/with/slash", "/starts/with/slash"),
            "/starts/with/slash/starts/with/slash");
  EXPECT_EQ(Path::Combine("ends/with/slash/", "ends/with/slash/"),
            "ends/with/slash/ends/with/slash/");
  EXPECT_EQ(Path::Combine("too/many/", "/slashes"), "too/many/slashes");
}

TEST_F(PathTest, CurrentPathTest) {
  EXPECT_EQ(Path::CurrentPath(), "/root/thilenius_bin/base");
}

TEST_F(PathTest, ExecutablePath) {
  EXPECT_EQ("/root/thilenius_bin/base/base_test", Path::ExecutablePath());
}

TEST_F(PathTest, ExistsTest) {
  EXPECT_TRUE(Path::Exists("/root/thilenius"));
  EXPECT_TRUE(Path::Exists(full_path_));
  EXPECT_TRUE(Path::Exists(rel_path_));
  EXPECT_FALSE(Path::Exists(""));
  EXPECT_FALSE(Path::Exists("/this/path/does/not/exist"));
}

TEST_F(PathTest, IsDirectoryTest) {
  EXPECT_TRUE(Path::IsDirectory(full_dir_));
  EXPECT_TRUE(Path::IsDirectory("/"));
  EXPECT_FALSE(Path::IsDirectory("/this/path/does/not/exist"));
  EXPECT_FALSE(Path::IsDirectory(full_path_));
}

TEST_F(PathTest, IsRegularFileTest) {
  EXPECT_FALSE(Path::IsRegularFile(full_dir_));
  EXPECT_FALSE(Path::IsRegularFile("/"));
  EXPECT_FALSE(Path::IsRegularFile("/this/path/does/not/exist"));
  EXPECT_TRUE(Path::IsRegularFile(full_path_));
}

TEST_F(PathTest, ParentPath) {
  EXPECT_EQ("some/path", Path::ParentPath("some/path/file.txt"));
  EXPECT_EQ("some/path", Path::ParentPath("some/path/file_no_ext"));
}

TEST_F(PathTest, WithoutSlashesTest) {
  EXPECT_EQ("some/path", Path::WithoutEdgeSlashes("//some/path///"));
}

TEST_F(PathTest, RelativePathTest) {
  EXPECT_EQ("the/final/path",
            Path::RelativePath("/the/root/the/final/path", "/the/root/"));
  EXPECT_EQ("the/final/path",
            Path::RelativePath("//the/root/the/final/path//", "the/root"));
  EXPECT_EQ("the/full/path", Path::RelativePath("/the/full/path", "/dne/"));
}

}  // namespace
}  // namespace base
}  // namespace thilenius
