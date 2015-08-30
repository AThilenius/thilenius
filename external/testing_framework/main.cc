//
//  main.cpp
//  UTFramework
//
//  Created by Alec Thilenius on 4/16/15.
//  Copyright (c) 2015 Thilenius. All rights reserved.
//
#include <iostream>

#include "test_runner.h"

SUITE(SuiteOne) {
  TEST("True Check", 5, 5) {
    runner->IsTrue(true, "Should be True", "Should NOT show up!");
    runner->IsTrue(true, "Should also be True", "Should NOT show up!");
  }

  TEST("False Check", 5, 5) {
    runner->IsTrue(false, "Should be False", "Should Be Shown");
    runner->IsTrue(false, "Should also be False", "Should Also Be Shown");
  }

  //TEST("Seg Fault Check", 5, 5) {
    //runner->IsTrue(true, "Should be True", "Should NOT show up!");
    //runner->IsTrue(false, "Should be False", "Should Be Shown");
    //runner->IsTrue(true, "Should Seg-Fault After This", "Should NOT show up!");
    //int foo = *((int*)0);
    //std::cout << foo;
  //}

  TEST("Infinite Loops Check", 5, 5) {
    runner->IsTrue(true, "Should be True", "Should NOT show up!");
    runner->IsTrue(false, "Should be False", "Should Be Shown");
    runner->IsTrue(true, "Should Timout After This", "Should NOT show up!");
    while (true) {
    }
  }

  TEST("Memory Leak Check Pass", 5, 5) {
    runner->IsTrue(true, "Should be True", "Should NOT show up!");
    runner->IsTrue(false, "Should be False", "Should Be Shown");
    int* foo = new int[1];
    delete foo;
  }

  TEST("Memory Leak Check Fail", 5, 5) {
    runner->IsTrue(true, "Should be True", "Should NOT show up!");
    runner->IsTrue(false, "Should be False", "Should Be Shown");
    int* foo = new int[1];
  }

  runner->GetConfig()->min_memory = 8;
  TEST("Min Memory Check", 5, 5) {
    runner->IsTrue(true, "Should be True", "Should NOT show up!");
    runner->IsTrue(false, "Should be False", "Should Be Shown");
    int* foo = new int[1];
    delete foo;
  }

  runner->GetConfig()->max_memory = 8;
  TEST("Max Memory Check", 5, 5) {
    runner->IsTrue(true, "Should be True", "Should NOT show up!");
    runner->IsTrue(false, "Should be False", "Should Be Shown");
    int* foo = new int[64];
    delete foo;
  }
}

int main(int argc, char* argv[]) {
  UTTestRunner runner (&argc, &argv);
  runner.RunSuite("Test Name", &SuiteOne);
}
