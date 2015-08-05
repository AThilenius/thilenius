#include <iostream>
#include <memory>
#include <string>

#include "base/gflags/gflags.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "base/log.h"
#include "base/macros.h"
#include "base/string.h"
#include "experimental/sample_assignment/linked_list.h"
#include "scorch/anvil/anvil.h"

using thilenius::scorch::anvil::Anvil;

class MockLinkedList : public LinkedList {
 public:
  MOCK_METHOD1(Push, void(int n));
  MOCK_METHOD0(Pop, int(void));
};

TEST(FirstAssignment, ShouldPass) {
}

TEST(SecondAssignment, UnknownExceptionTest) {
  throw "Unknown exception";
}

TEST(SecondAssignment, AssertTest) {
  ASSERT_EQ(1, 2) << "An error message!";
}

TEST(SecondAssignment, ShouldPass) {
}

int main(int argc, char** argv) {
   Anvil anvil(&argc, &argv);
   anvil.SetPoints("SecondAssignment", 10, 10);
   return anvil.Execute();
}
