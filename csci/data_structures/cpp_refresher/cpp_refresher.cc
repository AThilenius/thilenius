// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>

#include "cpp_refresher.h"

void PrintHelloWorld() {
  // Here you need to cout something (I recommend "Hello, world!" because
  // Computer Science, naturally).
  // Don't forget to include iostream!

  // Answer:
  std::cout << "Hello World" << std::endl;
}

void CallUberFunctions() {
  // There is a magic function, called "UberFunction" that is capable of taking
  // anything for an argument. You need to call "UberFunction" with the
  // following types: int, int* int** and char[]

  // The first one has been done for you:
  UberFunction(42);

  // Answer:
  int foo = 42;
  UberFunction(&foo);
  int* bar = &foo;
  UberFunction(&bar);
  char c;
  UberFunction(&c);
}

void Loops(int number_of_times) {
  // Call "UberFunction" without passing in anything, "number_of_times" times.
  // Some type of a loop is a good candidate for this.

  // Answer:
  for (int i = 0; i < number_of_times; i++) {
    DoesAwesomeLoopyThings();
  }
}

void CopyArrayOnHeap(const char* str) {
  // This ones a bit more tricky! The char* str here is a C style string.
  // Remember that C strings end in a NULL, aka a 0
  // Hint: You can find the length of the string knowing that piece of
  // information.
  // The goal is to create another C string (a char*) on the heap of the exact
  // same size as str, including the 0 at the end.
  // Then clean up memory by freeing the string on the heap.
  // You MUST use "new" and "delete", you cannot use malloc and free.

  // Answer:
  //int str_size = 0;
  //while (str[str_size] != 0) {
    //str_size++;
  //}
  //str_size += 1;
  //char* arr = new char[str_size];
  //delete arr;
}

void UseingObjects(UberClass& uber_class_1, UberClass* uber_class_2,
                   UberClass** uber_class_3) {
  // UbserCLass is a class with a member function (also called a method) called
  // "UberMethod" and a member variable (often just called a member) called
  // "uber_member".
  // For each of the instances of the UberClass that was passed in (aka. for
  // each of the objects) you need to call the member function (the method)
  // You also need to set the member variable to 42.

  // Answer:
  uber_class_1.UberMethod();
  uber_class_1.uber_member = 42;
  uber_class_2->UberMethod();
  uber_class_2->uber_member = 42;
  (*uber_class_3)->UberMethod();
  (*uber_class_3)->uber_member = 42;
}

// Extra Credit
void InPlaceReverse(char* str) {
  // This is a really tricky one! You must reverse the order of the C string
  // without allocating a new C string. Think about how to do this without
  // creating a second string, it's possible, I promise :)

  // Answer:
  int str_size = 0;
  while (str[str_size] != 0) {
    str_size++;
  }
  for (int i = 0; i < str_size / 2; i++) {
    char tmp = str[i];
    str[i] = str[str_size - 1 - i];
    str[str_size - 1 - i] = tmp;
  }
}
