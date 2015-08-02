#include <iostream>
#include <memory>
#include <string>

#include "base/gflags/gflags.h"
#include "base/glog/logging.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "base/macros.h"
#include "base/string.h"
#include "third_party/thrift/protocol/TBinaryProtocol.h"
#include "third_party/thrift/transport/TSocket.h"
#include "third_party/thrift/transport/TTransportUtils.h"
#include "scorch/anvil/anvil.h"

DEFINE_string(test, "Defaul value", "A test flag!");

namespace thilenius {
namespace scorch {
namespace anvil {

AnvilRunner::AnvilRunner(int argc, char** argv)
    : argc_(argc), argv_(argv) {
  INIT_GOOGLE(argc_, argv_);
}

int AnvilRunner::Execute() { return RUN_ALL_TESTS(); }

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
