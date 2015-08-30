// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/anvil.h"

#include <iostream>
#include <memory>
#include <string>

#include "base/file.h"
#include "base/gflags/gflags.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "base/log.h"
#include "scorch/anvil/test_harness.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/transport/TFileTransport.h"

DEFINE_string(project_state_path, "project_state.json",
              "The filename of the anvil ProjectState");

using ::apache::thrift::protocol::TJSONProtocol;
using ::apache::thrift::transport::TMemoryBuffer;
using ::thilenius::base::File;

namespace thilenius {
namespace scorch {
namespace anvil {



}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
