// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/anvil/anvil.h"

#include "scorch/anvil/test_harness.h"

using thilenius::scorch::anvil::Anvil;
using thilenius::scorch::anvil::TestHarness;

int main(int argc, char** argv) {
  TestHarness test_harness(
      "/root/thilenius_bin/experimental/cpp_refresher/cpp_refresher", true);
  test_harness.RunAllTests();
}
