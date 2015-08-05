// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/anvil.h"

#include <iostream>
#include <memory>
#include <string>

#include "base/gflags/gflags.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "scorch/anvil/test_harness.h"

namespace thilenius {
namespace scorch {
namespace anvil {

Anvil::Anvil(int* argc, char*** argv) : argc_(argc), argv_(argv) {
  gflags::ParseCommandLineFlags(argc, argv, true);
  testing::InitGoogleTest(argc, *argv);
  testing::InitGoogleMock(argc, *argv);
}

Anvil& Anvil::SetPoints(const std::string& test_name, int denominator,
                   int points_possible) {
  points_[test_name] = std::tuple<int, int>(denominator, points_possible);
  return *this;
}

int Anvil::Execute() {
  TestHarness harness(points_);
  harness.RunAllTests();
  return 0;
}

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius
