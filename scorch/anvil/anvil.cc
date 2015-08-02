#include <iostream>
#include <memory>
#include <string>

#include "/root/thilenius/base/gflags/gflags.h"
#include "/root/thilenius/base/glog/logging.h"
#include "/root/thilenius/base/gtest/gtest.h"
#include "/root/thilenius/base/string.h"
#include "/root/thilenius/third_party/thrift/protocol/TBinaryProtocol.h"
#include "/root/thilenius/third_party/thrift/transport/TSocket.h"
#include "/root/thilenius/third_party/thrift/transport/TTransportUtils.h"

DEFINE_string(test, "Defaul value", "A test flag!");

TEST(FactorialTest, Zero) { EXPECT_EQ(1, 1); }

int main(int argc, char **argv) {
  // Google Flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // GoogleLog
  google::InitGoogleLogging(argv[0]);
  FLAGS_colorlogtostderr = true;
  FLAGS_logtostderr = true;

  // GoogleTest
  ::testing::InitGoogleTest(&argc, argv);

  LOG(INFO) << "Hello from GLOG";
  std::shared_ptr<apache::thrift::transport::TSocket> socket(
      new apache::thrift::transport::TSocket("localhost", 9090));

  std::shared_ptr<std::string> test_ptr (new std::string("Hello, World!"));
  std::cout << *test_ptr << std::endl;

  return RUN_ALL_TESTS();
}
