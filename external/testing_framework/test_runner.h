// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_TESTING_FRAMEWORK_TEST_RUNNER_H_
#define EXTERNAL_TESTING_FRAMEWORK_TEST_RUNNER_H_

#include <functional>
#include <ostream>

#include "console_colors.h"
#include "test.h"

#define SUITE(suite_name) void suite_name(UTTestRunner* runner)
#define TEST(test_name, points_possible, points_denominator) \
  if (runner->_TestPass(test_name, points_possible, points_denominator))

extern const char* __file__;
extern size_t __line__;

void* operator new(size_t size);
void operator delete(void* ptr) noexcept;

#define new (__file__ = __FILE__, __line__ = __LINE__) && 0 ? NULL : new
#define delete (__file__ = __FILE__, __line__ = __LINE__), delete

class UTTestRunner {
 public:
  UTTestRunner(int* argc, char*** argv);
  void RunSuite(std::string suite_name,
                std::function<void(UTTestRunner*)> suite_function);
  void IsTrue(bool expression, std::string name, std::string assert_message);
  UTTestConfiguration* GetConfig();

  // Used by the testing framework itself to register tests
  bool _TestPass(const std::string& test_name, int points_possible,
                 int points_denominator);

 private:
  friend void SigSegVHandler(int signum);

  static void PrintRatioColor(int percent);
  static int GetPercent(int nominator, int denominator);
  // TestPass handlers
  bool ScrapeHandler(const std::string& test_name, int points_possible,
                     int points_denominator);
  bool RunHandler(const std::string& test_name, int points_possible,
                  int points_denominator);
  std::string WriteJson();

  // MiddleWare
  void StdExceptionCatcher(std::function<void(UTTestRunner*)> suite_function);
  void MemoryMonitor(std::function<void(UTTestRunner*)> suite_function);
  void SigSegCatcher(std::function<void(UTTestRunner*)> suite_function);
  void ThreadedTimeout(std::function<void(UTTestRunner*)> suite_function);

  UTTest* active_test_;
  bool export_json_;
  bool silent_;
  int active_test_index_;
  int current_test_index_;
  std::function<bool(const std::string&, int, int)> handler_;
  std::vector<UTTest> tests_;
};

#endif  // EXTERNAL_TESTING_FRAMEWORK_TEST_RUNNER_H_
