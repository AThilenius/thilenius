// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_TEST_HARNESS_H_
#define SCORCH_ANVIL_TEST_HARNESS_H_

#include <string>

#include "base/macros.h"
#include "base/value_of.hh"
#include "scorch/anvil/anvil_types.h"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace scorch {
namespace anvil {

class TestHarness {
 public:
  TestHarness(const std::string& runnable_path, bool silent);
  ValueOf<::anvil::TestRunReport> RunAllTests();

 private:
  DISALLOW_COPY_AND_ASSIGN(TestHarness);

  std::string runnable_path_;
  bool silent_;
};  // class TestHarness

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_TEST_HARNESS_H_
