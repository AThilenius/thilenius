#include <iostream>
#include <memory>
#include <string>

#include "base/gflags/gflags.h"
#include "base/glog/logging.h"
#include "base/gmock/gmock.h"
#include "base/gtest/gtest.h"
#include "base/macros.h"
#include "base/string.h"
#include "experimental/sample_assignment/linked_list.h"
#include "scorch/anvil/anvil.h"

class MockLinkedList : public LinkedList {
 public:
  MOCK_METHOD1(Push, void(int n));
  MOCK_METHOD0(Pop, int(void));
};

TEST(LinkedListTest, CanPush) {
  LinkedList list;
  list.Push(0);
}

TEST(LinkedListTest, CanPop) {
  LinkedList list;
  list.Push(0);
  list.Pop();
}

int main(int argc, char** argv) {
  thilenius::scorch::anvil::AnvilRunner runner(argc, argv);
  return runner.Execute();
}
