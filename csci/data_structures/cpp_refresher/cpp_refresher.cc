// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>

#include "cpp_refresher.h"

void PrintHelloWorld() {
  // Here you need to cout something (I recommend "Hello, world!" because
  // Computer Science).
  // Don't forget to include iostream!
  std::cout << "Hello World" << std::endl;
}

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

void UseingObjects(UberClass& uber_class_1, UberClass* uber_class_2,
                   UberClass** uber_class_3) {
  uber_class_1.UberMethod();
  uber_class_1.uber_member = 42;
  uber_class_2->UberMethod();
  uber_class_2->uber_member = 42;
  (*uber_class_3)->UberMethod();
  (*uber_class_3)->uber_member = 42;
}

void InPlaceReverse(char* str) {
  std::cout << "Before:[" << str << "]" << std::endl;
  int str_size = 0;
  while (str[str_size] != 0) {
    str_size++;
  }
  for (int i = 0; i < str_size / 2; i++) {
    char tmp = str[i];
    str[i] = str[str_size - 1 - i];
    str[str_size - 1 - i] = tmp;
  }
  std::cout << "After: [" << str << "]" << std::endl;
}
