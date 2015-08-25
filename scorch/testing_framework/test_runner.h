//
//  UTTestRunner.h
//  UTFramework
//
//  Created by Alec Thilenius on 4/20/15.
//  Copyright (c) 2015 Thilenius. All rights reserved.
//
#pragma once

#include <functional>

#include "console_colors.h"
#include "test.h"

#define SUITE(suite_name) void suite_name(UTTestRunner* runner)
#define TEST(test_name, points_possible, points_denominator) \
  if (runner->_TestPass(test_name, points_possible, points_denominator))

extern const char* __file__;
extern size_t __line__;

// void* operator new(size_t size);
// void operator delete(void* ptr) noexcept;

#define new (__file__ = __FILE__, __line__ = __LINE__) && 0 ? NULL : new
#define delete (__file__ = __FILE__, __line__ = __LINE__), delete

class UTTestRunner {
 public:
  void RunSuite(std::string suiteName,
                std::function<void(UTTestRunner*)> suiteFunction);
  void IsTrue(bool expression, std::string name, std::string assertMessage);
  UTTestConfiguration* GetConfig();

  // Used by the testing framework itself to register tests
  bool _TestPass(const std::string& testName, int points_possible,
                 int points_denominator);

 private:
  static void PrintRatioColor(int percent);
  static int GetPercent(int nominator, int denominator);
  // TestPass handlers
  bool ScrapeHandler(const std::string& test_name, int points_possible,
                     int points_denominator);
  bool RunHandler(const std::string& test_name, int points_possible,
                  int points_denominator);

  // MiddleWare
  void StdExceptionCatcher(std::function<void(UTTestRunner*)> suiteFunction);
  void MemoryMonitor(std::function<void(UTTestRunner*)> suiteFunction);
  void SigSegCatcher(std::function<void(UTTestRunner*)> suiteFunction);
  void ThreadedTimeout(std::function<void(UTTestRunner*)> suiteFunction);

  friend void SigSegVHandler(int signum);

 private:
  std::vector<UTTest> m_tests;
  std::function<bool(const std::string&, int, int)> m_handler;
  UTTest* m_activeTest;
  int m_activeTestIndex;
  int m_currentTestIndex;
};
