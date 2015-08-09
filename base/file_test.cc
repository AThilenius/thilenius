// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <chrono>
#include <cstdio>
#include <fstream>
#include <thread>

#include "base/file.h"
#include "base/gtest/gtest.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

class FileTest : public testing::Test {
 protected:
  FileTest()
      : file_cont_("Hello, World!"),
        full_path_("/root/thilenius_bin/base/test_file.txt"),
        rel_path_("test_file.txt") {}

  virtual void SetUp() { CreateFile(); }

  virtual void TearDown() { RemoveFile(); }

  void CreateFile() {
    std::remove(rel_path_.c_str());
    std::ofstream test_file(rel_path_);
    test_file << file_cont_;
  }

  void RemoveFile() { std::remove(rel_path_.c_str()); }

  std::string ReadFile() {
    std::ifstream file_stream(rel_path_);
    std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
                              std::istreambuf_iterator<char>());
    return std::move(file_contents);
  }

  std::string file_cont_;
  std::string full_path_;
  std::string rel_path_;
};

TEST_F(FileTest, AppendToFileTest) {
  std::string append_text = ":Appended Test.";
  std::string full_text = StrCat(file_cont_, append_text);
  EXPECT_TRUE(File::AppendToFile(rel_path_, append_text));
  EXPECT_EQ(full_text, ReadFile())
      << "Append did not correctly append to an existing file.";
  RemoveFile();
  EXPECT_TRUE(File::AppendToFile(rel_path_, append_text));
  EXPECT_EQ(append_text, ReadFile())
      << "Append did not correctly append to a new file.";
}

TEST_F(FileTest, ExistsTest) {
  EXPECT_TRUE(File::Exists(rel_path_))
      << "File::Exists returned false for a file that that exists.";
  EXPECT_TRUE(File::Exists(full_path_))
      << "File::Exists returned false for a file that that exists.";
  EXPECT_FALSE(File::Exists("this_file_does_not_exist"))
      << "File::Exists returned true for a file that does not exist.";
}

TEST_F(FileTest, MD5OrDieTest) {
  std::string md5 = "65a8e27d8879283831b664bd8b7f0ad4";
  EXPECT_EQ(md5, File::MD5OrDie(rel_path_));
  EXPECT_EQ(md5, File::MD5OrDie(full_path_));
  EXPECT_EXIT(File::MD5OrDie("file_does_not_exist"),
              ::testing::ExitedWithCode(EXIT_FAILURE),
              ".*Failed to find file.*");
}

TEST_F(FileTest, LastWriteTimeTest) {
  RemoveFile();
  time_t before_write;
  std::time(&before_write);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  {
    std::ofstream file_stream(rel_path_);
    file_stream << "An Update";
  }
  time_t update_time = File::LastWriteTime(rel_path_);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  time_t after_time;
  std::time(&after_time);
  EXPECT_LE(before_write, update_time);
  EXPECT_LE(update_time, after_time);
}

TEST_F(FileTest, SizeTest) {
  EXPECT_EQ(File::Size(rel_path_), 13);
  EXPECT_EQ(File::Size(full_path_), 13);
}

TEST_F(FileTest, ReadContentsOrDieTest) {
  EXPECT_TRUE(File::ReadContentsOrDie(rel_path_).length() != 0)
      << "File::ReadContentsOrDie returned a zero length string for an "
         "existing text file.";
  EXPECT_TRUE(File::ReadContentsOrDie(full_path_).length() != 0)
      << "File::ReadContentsOrDie returned a zero length string for an "
         "existing text file.";
  EXPECT_EXIT(File::ReadContentsOrDie("file_does_not_exist"),
              ::testing::ExitedWithCode(EXIT_FAILURE),
              ".*Failed to find file.*");
}

}  // namespace
}  // namespace base
}  // namespace thilenius
