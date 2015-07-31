#include <iostream>
#include "base/gflags/gflags.h"
#include "base/glog/logging.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "base/string.h"

DEFINE_string(test, "Defaul value", "A test flag!");

TEST(FactorialTest, Zero) { EXPECT_EQ(1, 1); }

int main(int argc, char **argv) {
  // Google Flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // GoogleLog
  google::InitGoogleLogging(argv[0]);
  FLAGS_colorlogtostderr = true;
  FLAGS_logtostderr = true;

  // GoogleTest / GoogleMcok
  ::testing::InitGoogleMock(&argc, argv);
  ::testing::InitGoogleTest(&argc, argv);

  LOG(INFO) << "Hello from GLOG";

  return RUN_ALL_TESTS();
}