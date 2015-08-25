// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>

#include "base/log.h"
#include "cpp_refresher.h"

class UberClass : public Base {
 public:
  void foo(int f) {
    Base::foo(f);
  }
};

// IO
void PrintHelloWorld() {
  // Here you need to cout something (I recommend "Hello, world!" because
  // Computer Science).
  // Don't forget to include iostream!
  std::cout << "Hello World" << std::endl;
}

// Calling functions
// Calling UberFunction wtih different args
// Calling a namespaced function
// Calling a member function
// Calling a static member function
void CallUberFunctions() {
  // There is a magic function, called "UberFunction" that is capable of taking
  // anything for an argument. You need to call "UberFunction" with the
  // following types: int, int* int** and char[]

  // The first one has been done for you:
  int foo = 42;
  UberFunction(42);
  UberFunction(&foo);
  int* bar = &foo;
  UberFunction(&bar);
  char c;
  UberFunction(&c);
}

void Loops(int number_of_times) {
  // Call "UberFunction" without passing in anything, "number_of_times" times.
}

void CopyArrayOnHeap(const char* array) {
  int str_size = 0;
  while (array[str_size] != 0) {
    str_size++;
  }
  str_size += 1;
  char* arr = new char[str_size];
  delete arr;
}
