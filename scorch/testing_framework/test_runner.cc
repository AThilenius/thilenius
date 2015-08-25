//
//  UTTestRunner.cpp
//  UTFramework
//
//  Created by Alec Thilenius on 4/20/15.
//  Copyright (c) 2015 Thilenius. All rights reserved.
//
#include "scorch/testing_framework/test_runner.h"

#undef new
#undef delete

#include <chrono>
#include <math.h>
#include <mutex>
#include <signal.h>
#include <sstream>
#include <thread>

std::function<void()> g_testIterateFunction;
std::function<void()> g_testEndFunction;
std::function<void*(size_t)> g_newHandler;
std::function<void(void*)> g_deleteHandler;

// These must be global because the stack ?can? be unwound when catching a
// sig-segv.
UTTestRunner* g_testRunner;
std::string g_suiteName;
std::function<void(UTTestRunner*)> g_suiteFunction;

// Used to protect the test-end print
std::mutex g_printLock;

// Use by the new / delete overload macros
const char* __file__ = "unknown";
size_t __line__ = -1;

// Must use accessors and scoped statics for anything used by new and delete
bool& HandlersReadyAccessor() {
  static bool handlers_ready = false;
  return handlers_ready;
}

bool& CustomHanderActiveAccessor() {
  static bool custom_handler_active = false;
  return custom_handler_active;
}

void* operator new(size_t size) {
  if (!HandlersReadyAccessor() || CustomHanderActiveAccessor()) {
    return malloc(size);
  } else {
    CustomHanderActiveAccessor() = true;
    void* ptr = g_newHandler(size);
    __file__ = "unknown";
    __line__ = -1;
    CustomHanderActiveAccessor() = false;
    return ptr;
  }
}

void operator delete(void* ptr) noexcept {
  static bool custom_handler_active = false;
  if (!HandlersReadyAccessor() || CustomHanderActiveAccessor()) {
    free(ptr);
  } else {
    CustomHanderActiveAccessor() = true;
    g_deleteHandler(ptr);
    __file__ = "unknown";
    __line__ = -1;
    CustomHanderActiveAccessor() = false;
  }
}

void UTTestRunner::RunSuite(std::string suiteName,
                            std::function<void(UTTestRunner*)> suiteFunction) {
  // Use global variables to prevent stack unwinding issues from sig-segv
  g_testRunner = this;
  g_suiteName = suiteName;
  g_suiteFunction = suiteFunction;

  // Set up the first UTTest now, to allow for the Config to be changed
  m_tests.push_back(UTTest());

  // Bind the scrap handler
  m_handler =
      std::bind(&UTTestRunner::ScrapeHandler, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
  // Run the Scrape phase
  suiteFunction(this);
  // Bind the runner handler
  m_handler = std::bind(&UTTestRunner::RunHandler, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3);
  m_activeTestIndex = -1;
  // Create the iterate function (can then be used by the POSIX SIGSEGV handler)
  g_testIterateFunction = []() {
    g_testRunner->m_currentTestIndex = 0;
    g_testRunner->m_activeTestIndex++;
    for (g_testRunner->m_activeTest = 0;
         g_testRunner->m_activeTestIndex < g_testRunner->m_tests.size() - 1;
         g_testRunner->m_activeTestIndex++) {
      g_testRunner->m_activeTest =
          &g_testRunner->m_tests[g_testRunner->m_activeTestIndex];
      // Need to load the new config here. Might want to use a RACK format for
      // this...
      g_testRunner->SigSegCatcher(g_suiteFunction);

      g_testRunner->m_currentTestIndex = 0;
    }
  };

  // Create the end function (can then be used by the POSIX SIGSEGV handler)
  g_testEndFunction = []() {
    g_printLock.lock();
    g_testEndFunction = []() {};
    g_printLock.unlock();
    // Collect metrics
    bool didPass = true;
    int points_earned = 0;
    int points_possible = 0;
    int points_denominator = 0;
    int points_possible_extra_credit = 0;
    for (int i = 0; i < g_testRunner->m_tests.size() - 1; i++) {
      UTTest* test = &g_testRunner->m_tests[i];
      if (!test->DidPass()) {
        didPass = false;
      }
      points_earned += test->PointsEarned();
      points_possible += test->points_possible;
      points_denominator += test->points_denominator;
    }
    std::cout << std::endl;
    std::cout << Blue << "Suite: " << g_suiteName << std::endl;
    std::cout << Blue << "|" << std::endl;
    for (int i = 0; i < g_testRunner->m_tests.size() - 1; i++) {
      UTTest* test = &g_testRunner->m_tests[i];
      test->Print();
    }
    if (didPass) {
      std::cout << Green << "Passed!" << White << std::endl;
    } else {
      std::cout << Red << "Failed!" << White << std::endl;
    }
    // Print report card
    if (points_possible > 0) {
      std::cout << White << std::endl;
      std::cout << Blue << "Points: ";
      int earned_percent_total = GetPercent(points_earned, points_denominator);
      PrintRatioColor(earned_percent_total);
      std::cout << points_earned << "/" << points_denominator << ", "
                << earned_percent_total << "%" << std::endl;
      std::cout << Blue << "|" << std::endl;
      for (int i = 0; i < g_testRunner->m_tests.size() - 1; i++) {
        UTTest& test = g_testRunner->m_tests[i];
        std::cout << Blue << "|   ";
        int earned_percent =
            GetPercent(test.PointsEarned(), test.points_denominator);
        PrintRatioColor(earned_percent);
        std::cout << test.PointsEarned() << "/" << test.points_denominator
                  << Blue << " - " << test.Name << std::endl;
      }
      std::cout << Blue << "|" << std::endl;
      std::cout << Blue << "[";
      PrintRatioColor(earned_percent_total);
      for (int i = 0; i < 50; i++) {
        if (round(earned_percent_total / 2.0f) > i) {
          std::cout << "=";
        } else {
          std::cout << " ";
        }
      }
      if (points_possible > points_denominator) {
        std::cout << Blue << "|" << Green;
        int extra_credit_points = points_possible - points_denominator;
        int extra_credit_percent =
            GetPercent(extra_credit_points, points_denominator);
        int earned_extra_credit_percent = earned_percent_total - 100;
        for (int i = 0; i < extra_credit_percent; i++) {
          if (earned_extra_credit_percent > i) {
            std::cout << "=";
          } else {
            std::cout << " ";
          }
        }
      }
      std::cout << Blue << "]" << White << std::endl;
      std::cout << std::endl;
    }
    std::cout.flush();
  };

  // Funally call both
  g_testIterateFunction();
  g_testEndFunction();
}

void UTTestRunner::IsTrue(bool expression, std::string name,
                          std::string assertMessage) {
  // Add the condition
  m_activeTest->Conditions.push_back(
      UTCondition(expression, name, assertMessage));
}

UTTestConfiguration* UTTestRunner::GetConfig() {
  return &(m_tests[m_tests.size() - 1]).Configuration;
}

bool UTTestRunner::_TestPass(const std::string& testName, int points_possible,
                             int points_denominator) {
  return m_handler(testName, points_possible, points_denominator);
}

void UTTestRunner::PrintRatioColor(int percent) {
  if (percent < 70) {
    std::cout << Red;
  } else if (percent < 90) {
    std::cout << Yellow;
  } else {
    std::cout << Green;
  }
}

int UTTestRunner::GetPercent(int nominator, int denominator) {
  if (denominator == 0) {
    return 100;
  }
  return static_cast<int>(100.0f * static_cast<float>(nominator) /
                          static_cast<float>(denominator));
}

// _TestPass handlers
bool UTTestRunner::ScrapeHandler(const std::string& test_name,
                                 int points_possible, int points_denominator) {
  // Update the last UTTest
  UTTest& test = m_tests[m_tests.size() - 1];
  test.Name = test_name;
  test.points_possible = points_possible;
  test.points_denominator = points_denominator;

  // Create a new config for the next test
  m_tests.push_back(UTTest());

  // Don't run the test
  return false;
}

bool UTTestRunner::RunHandler(const std::string& test_name, int points_possible,
                              int points_denominator) {
  // Returns true for ONLY the active test
  if (m_currentTestIndex == m_activeTestIndex) {
    m_currentTestIndex++;
    return true;
  }

  m_currentTestIndex++;
  return false;
}

void UTTestRunner::StdExceptionCatcher(
    std::function<void(UTTestRunner*)> suiteFunction) {
  try {
    suiteFunction(this);
  } catch (...) {
    m_activeTest->FatalMessage = "A fatal exception was caught!";
  }
}

void UTTestRunner::MemoryMonitor(
    std::function<void(UTTestRunner*)> suiteFunction) {
  // Register Custom handlers for new / delete
  g_newHandler = [this](size_t size) -> void* {
    void* ptr = malloc(size);

    if (__line__ != -1) {
      this->m_activeTest->RegisterAllocation(
          MemoryAllocation(ptr, size, __file__, __line__));
    }

    return ptr;
  };

  g_deleteHandler = [this](void* ptr) -> void {
    if (__line__ != -1) {
      this->m_activeTest->RegisterFree(ptr);
    }

    free(ptr);
  };
  HandlersReadyAccessor() = true;

  StdExceptionCatcher(suiteFunction);
}

void UTTestRunner::ThreadedTimeout(
    std::function<void(UTTestRunner*)> suiteFunction) {
  std::cout.flush();
  bool threadFinished = false;
  std::thread workerThread([this, &suiteFunction, &threadFinished]() {
    // No need to syncronize this
    this->MemoryMonitor(suiteFunction);
    threadFinished = true;
  });
  // workerThread.detach();

  // Again, no need to syncronize this, there are no possible race conditions
  // that I care about
  static bool warning_issued = false;
  auto warn_time =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
  auto endTime = std::chrono::steady_clock::now() + std::chrono::seconds(4);
  while (threadFinished == false &&
         std::chrono::steady_clock::now() < endTime) {
    if (std::chrono::steady_clock::now() > warn_time && !warning_issued) {
      std::cout << Yellow
                << "One of the tests is taking an unusually long time...";
      warning_issued = true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Check if we timed out
  if (std::chrono::steady_clock::now() >= endTime) {
    workerThread.detach();
    m_activeTest->FatalMessage =
        "Timed out, likley an infite loop or waiting for user input!";
  } else {
    workerThread.join();
  }

  std::cout.flush();
}

void SigSegVHandler(int signum) {
  std::cout << "SIG-SEG Handler" << std::endl;

  // Re-Register the SIG-SEGV handler
  signal(SIGSEGV, SigSegVHandler);

  g_testRunner->m_activeTest->FatalMessage =
      "SEG-FAULT!\nYou are likley trying to dereference a null pointer.";
  g_testIterateFunction();
  g_testEndFunction();
  std::cout.flush();
  signal(signum, SIG_DFL);
}

void UTTestRunner::SigSegCatcher(
    std::function<void(UTTestRunner*)> suiteFunction) {
  signal(SIGSEGV, SigSegVHandler);
  ThreadedTimeout(suiteFunction);
}
