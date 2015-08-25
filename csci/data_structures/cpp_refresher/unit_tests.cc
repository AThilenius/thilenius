// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>
#include <unordered_set>

#include "cpp_refresher.h"
#include "scorch/testing_framework/test_runner.h"

namespace {

bool other_uber_function_called = false;
int does_loopy_things_count = 0;
std::unordered_set<std::string> type_call_set_;
int last_new_size_ = 0;
void* last_new_ptr_ = nullptr;
void* last_delete_ptr_ = nullptr;

}  // namespace

void PointersTestAddCall(const std::string& type_name) {
  type_call_set_.insert(type_name);
}

int PrintHelloWorldHarness() {
  std::stringstream buffer;
  std::streambuf* sbuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());
  PrintHelloWorld();
  int count = buffer.str().length();
  std::cout.rdbuf(sbuf);
  return count;
}

void DoesAwesomeLoopyThings() { does_loopy_things_count++; }

namespace uber_namespace {
void OtherUberFunction() { other_uber_function_called = true; }
}

// namespace {

std::string GetRandomString() {
  std::string string;
  int size = 2 + (std::rand() % 20);
  string.reserve(size);
  for (int i = 0; i < size; i++) {
    string[i] = 65 + (std::rand() % 24);
  }
  return std::move(string);
}

SUITE(CPlusPlusRefresher) {
  TEST("PrintHelloWorld", 5, 0) {
    int number_of_chars_couted = PrintHelloWorldHarness();
    runner->IsTrue(number_of_chars_couted > 0, "Was something printed",
                   "You need to print something to cout");
  }

  TEST("Call the UberFunction with different types", 20, 20) {
    CallUberFunctions();
    runner->IsTrue(
        type_call_set_.find(typeid(int).name()) != type_call_set_.end(),
        "Was uber function called with int",
        "You need to call the "
        "function \"UberFunction\" "
        "with an int argument");
    runner->IsTrue(
        type_call_set_.find(typeid(int*).name()) != type_call_set_.end(),
        "Was uber function called with int*",
        "You need to call the "
        "function \"UberFunction\" "
        "with an int* argument");
    runner->IsTrue(
        type_call_set_.find(typeid(int**).name()) != type_call_set_.end(),
        "Was uber function called with int**",
        "You need to call the "
        "function \"UberFunction\" "
        "with an int** argument");
    runner->IsTrue(
        type_call_set_.find(typeid(char*).name()) != type_call_set_.end(),
        "Was uber function called with char*",
        "You need to call the "
        "function \"UberFunction\" "
        "with a char* argument");
  }

  // TEST("CallDoesAwesomeLoopyThings1Times") {
  // does_loopy_things_count = 0;
  // Loops(1);
  // EXPECT_EQ(1, does_loopy_things_count)
  //<< "\"DoesAwesomeLoopyThings\" should have been called once.";
  //}

  // TEST("CallDoesAwesomeLoopyThings42Times") {
  // does_loopy_things_count = 0;
  // Loops(42);
  // EXPECT_EQ(42, does_loopy_things_count)
  //<< "\"DoesAwesomeLoopyThings\" should have been called 42 times.";
  //}

  // TEST("CallDoesAwesomeLoopyThings69Times") {
  // does_loopy_things_count = 0;
  // Loops(69);
  // EXPECT_EQ(69, does_loopy_things_count)
  //<< "\"DoesAwesomeLoopyThings\" should have been called 69 times.";
  // does_loopy_things_count = 0;
  //}

  // TEST("CallDoesAwesomeLoopyThingsNegativeTimes") {
  // does_loopy_things_count = 0;
  // Loops(-1);
  // EXPECT_EQ(0, does_loopy_things_count)
  //<< "\"DoesAwesomeLoopyThings\" should not have been called when a "
  //"negative number was passed in for the number of times.";
  //}

  // TEST("NewCorrectSize") {
  // int size = std::rand() % 100;
  // char* array = new char[size];
  // for (int i = 0; i < size; i++) {
  // array[i] = 1 + (rand() % 100);
  //}
  // array[size - 1] = 0;
  // last_new_size_ = 0;
  // CopyArrayOnHeap(array);
  // int new_call_size = last_new_size_;
  // EXPECT_EQ(size, new_call_size)
  //<< "You should have allocated " << size
  //<< " bytes because that's how big the char[] was";
  //}

  // TEST("DeleteCorrectPointer") {
  // std::string random_string = GetRandomString();
  // last_new_ptr_ = nullptr;
  // last_delete_ptr_ = nullptr;
  // CopyArrayOnHeap(random_string.c_str());
  // void* last_new_ptr = last_new_ptr_;
  // void* last_delete_ptr = last_delete_ptr_;
  // EXPECT_EQ(last_new_ptr, last_delete_ptr)
  //<< "You need to call delete on memory that was allocated on the heap";
  //}

  // TEST("CopiedStringCorrectly") {
  // std::string random_string = GetRandomString();
  // last_new_size_ = 0;
  // last_new_ptr_ = nullptr;
  // last_delete_ptr_ = nullptr;
  // CopyArrayOnHeap(random_string.c_str());
  // int last_new_size = last_new_size_;
  // void* last_new_ptr = last_new_ptr_;
  // void* last_delete_ptr = last_delete_ptr_;
  // EXPECT_EQ(last_new_ptr, last_delete_ptr)
  //<< "You need to make sure you're allocating and deleteing memory "
  //"correctly before doing this step";
  // EXPECT_EQ(last_new_ptr, last_delete_ptr)
  //<< "You need to make sure you're allocating the correct amount of "
  //"memory "
  //"before doing this step";
  // if (last_new_ptr != last_delete_ptr ||
  // last_new_size != random_string.size()) {
  // return;
  //}
  // std::string string_copy =
  // std::string(static_cast<char*>(last_delete_ptr), random_string.size());
  // EXPECT_EQ(random_string, string_copy)
  //<< "You did not correctly copy the char*";
  //}
}

//}  // namespace

int main(int argc, const char* argv[]) {
  UTTestRunner runner;
  runner.RunSuite("Test Name", &CPlusPlusRefresher);
}
