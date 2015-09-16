// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <boost/filesystem.hpp>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <thread>

#include "base/directory.h"
#include "base/file.h"
#include "base/gtest/gtest.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

class DirectoryTest : public testing::Test {
 protected:
  DirectoryTest()
      : dir_name_("/root/thilenius_bin/base/new_dir"),
        rename_dir_name_("/root/thilenius_bin/base/new_dir_rename"),
        sub_dir_name_("/root/thilenius_bin/base/new_dir/sub_dir") {}

  //virtual void TearDown() { ::boost::filesystem::remove_all(dir_name_); }

  std::string dir_name_;
  std::string rename_dir_name_;
  std::string sub_dir_name_;
};

TEST_F(DirectoryTest, CreateExistsRemoveTest) {
  ASSERT_FALSE(Directory::Exists(dir_name_));
  ASSERT_TRUE(Directory::Create(dir_name_));
  ASSERT_TRUE(Directory::Exists(dir_name_));
  ASSERT_TRUE(Directory::Remove(dir_name_));
  ASSERT_FALSE(Directory::Exists(dir_name_));
}

TEST_F(DirectoryTest, CreateExistsRemoveGetChildrenRecursiveTest) {
  ASSERT_FALSE(Directory::Exists(dir_name_));
  ASSERT_FALSE(Directory::Exists(sub_dir_name_));
  // Create
  ASSERT_TRUE(Directory::Create(dir_name_));
  ASSERT_TRUE(Directory::Exists(dir_name_));
  ASSERT_TRUE(Directory::Create(sub_dir_name_));
  ASSERT_TRUE(Directory::Exists(sub_dir_name_));
  // Get Children
  ASSERT_EQ(Directory::GetChildren(dir_name_).size(), 1);
  ASSERT_GT(Directory::GetChildren("/root/thilenius").size(), 2);
  ASSERT_EQ(Directory::GetChildrenDirectories(dir_name_).size(), 1);
  ASSERT_GT(Directory::GetChildrenDirectories("/root/thilenius").size(), 2);
  ASSERT_EQ(Directory::GetChildrenFiles(dir_name_).size(), 0);
  ASSERT_GT(Directory::GetChildrenFiles("/root/thilenius/base").size(), 2);
  // Remove
  ASSERT_TRUE(Directory::RemoveRecursive(dir_name_));
  ASSERT_FALSE(Directory::Exists(sub_dir_name_));
  ASSERT_FALSE(Directory::Exists(dir_name_));
}

TEST_F(DirectoryTest, RenameTest) {
  ASSERT_FALSE(Directory::Exists(dir_name_));
  ASSERT_TRUE(Directory::Create(dir_name_));
  ASSERT_TRUE(Directory::Exists(dir_name_));
  ASSERT_TRUE(Directory::Rename(dir_name_, "new_dir_rename"));
  ASSERT_TRUE(Directory::Exists(rename_dir_name_));
  ASSERT_TRUE(Directory::Remove(rename_dir_name_));
  ASSERT_FALSE(Directory::Exists(rename_dir_name_));
}

TEST_F(DirectoryTest, RecuriveGetTests) {
  EXPECT_TRUE(Directory::Create(dir_name_));
  EXPECT_TRUE(File::WriteToFile(StrCat(dir_name_, "/file1.txt"), "Hello"));
  EXPECT_TRUE(Directory::Create(StrCat(dir_name_, "/subDir1")));
  EXPECT_TRUE(File::WriteToFile(StrCat(dir_name_, "/subDir1/file2.txt"), "Hello"));
  EXPECT_TRUE(Directory::Create(StrCat(dir_name_, "/subDir2")));
  EXPECT_TRUE(Directory::Create(StrCat(dir_name_, "/subDir2/subSubDir1")));
  EXPECT_TRUE(File::WriteToFile(StrCat(dir_name_, "/subDir2/file3.txt"), "Hello"));
  for (const auto& path : Directory::GetChildrenRecursive(dir_name_)) {
    std::cout << path << std::endl;
  }
  ASSERT_EQ(6, Directory::GetChildrenRecursive(dir_name_).size());
  ASSERT_EQ(3, Directory::GetChildrenDirectoriesRecursive(dir_name_).size());
  ASSERT_EQ(3, Directory::GetChildrenFilesRecursive(dir_name_).size());
}

}  // namespace
}  // namespace base
}  // namespace thilenius
