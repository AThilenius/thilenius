// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CSCI_DATA_STRUCTURES_CPP_REFRESHER_CPP_REFRESHER_H_
#define CSCI_DATA_STRUCTURES_CPP_REFRESHER_CPP_REFRESHER_H_

#include <string>
#include <typeinfo>

void PointersTestAddCall(const std::string& type_name);
int PrintHelloWorldHarness();

template <typename T>
void UberFunction(T val) {
  PointersTestAddCall(typeid(T).name());
}

void DoesAwesomeLoopyThings();

void PrintHelloWorld();

void CopyArrayOnHeap(const char* array);

void CallUberFunctions();

void Loops(int number_of_times);

namespace uber_namespace {
void OtherUberFunction();
}

class Base {
 public:
  void foo(int f) { }
};

#endif  // CSCI_DATA_STRUCTURES_CPP_REFRESHER_CPP_REFRESHER_H_
