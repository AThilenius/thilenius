// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "test_runner.h"

#undef new
#undef delete

#include <chrono>
#include <iomanip>
#include <math.h>
#include <mutex>
#include <signal.h>
#include <sstream>
#include <thread>

#include "json.h"

namespace {

UTTestRunner* test_runner_;
std::function<void()> test_end_function_;
std::function<void()> test_iterate_function_;
std::function<void(UTTestRunner*)> suite_function_;
std::function<void(void*)> delete_handler_;
std::function<void*(size_t)> new_handler_;
std::ostream* stream_ = &std::cout;
std::string suite_name_;
std::stringstream string_stream_;
bool exporting_json_;

}  // namespace

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
  static bool custohandler__active = false;
  return custohandler__active;
}

void* operator new(size_t size) {
  if (!HandlersReadyAccessor() || CustomHanderActiveAccessor()) {
    return malloc(size);
  } else {
    CustomHanderActiveAccessor() = true;
    void* ptr = new_handler_(size);
    __file__ = "unknown";
    __line__ = -1;
    CustomHanderActiveAccessor() = false;
    return ptr;
  }
}

void operator delete(void* ptr) noexcept {
  static bool custohandler__active = false;
  if (!HandlersReadyAccessor() || CustomHanderActiveAccessor()) {
    free(ptr);
  } else {
    CustomHanderActiveAccessor() = true;
    delete_handler_(ptr);
    __file__ = "unknown";
    __line__ = -1;
    CustomHanderActiveAccessor() = false;
  }
}

UTTestRunner::UTTestRunner(int* argc, char*** argv)
    : active_test_(nullptr),
      export_json_(false),
      silent_(false),
      active_test_index_(-1),
      current_test_index_(-1) {
  // Parse out flags we are looking for
  std::vector<std::string> args;
  std::vector<std::string> remaining_args;
  for (int i = 0; i < *argc; i++) {
    args.push_back((*argv)[i]);
  }
  for (const auto& arg : args) {
    if (arg == "--export_json" || arg == "-export_json" || arg == "-e") {
      export_json_ = true;
    } else if (arg == "--silent" || arg == "-silent" || arg == "-s") {
      silent_ = true;
      stream_ = &string_stream_;
    } else {
      remaining_args.push_back(arg);
    }
  }
  // TODO(athilenius): Remove args we needed from argc/argv
}

void UTTestRunner::RunSuite(std::string suitename,
                            std::function<void(UTTestRunner*)> suiteFunction) {
  // Use global variables to prevent stack unwinding issues from sig-segv
  test_runner_ = this;
  suite_name_ = suitename;
  suite_function_ = suiteFunction;
  exporting_json_ = export_json_;

  // Set up the first UTTest now, to allow for the Config to be changed
  tests_.push_back(UTTest());

  // Bind the scrap handler
  handler_ =
      std::bind(&UTTestRunner::ScrapeHandler, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
  // Run the Scrape phase
  suiteFunction(this);
  // Bind the runner handler
  handler_ = std::bind(&UTTestRunner::RunHandler, this, std::placeholders::_1,
                       std::placeholders::_2, std::placeholders::_3);
  active_test_index_ = -1;
  // Create the iterate function (can then be used by the POSIX SIGSEGV handler)
  test_iterate_function_ = []() {
    test_runner_->current_test_index_ = 0;
    test_runner_->active_test_index_++;
    for (test_runner_->active_test_ = 0;
         test_runner_->active_test_index_ < test_runner_->tests_.size() - 1;
         test_runner_->active_test_index_++) {
      test_runner_->active_test_ =
          &test_runner_->tests_[test_runner_->active_test_index_];
      // Need to load the new config here. Might want to use a RACK format for
      // this...
      test_runner_->SigSegCatcher(suite_function_);

      test_runner_->current_test_index_ = 0;
    }
  };

  // Create the end function (can then be used by the POSIX SIGSEGV handler)
  test_end_function_ = []() {
    g_printLock.lock();
    test_end_function_ = []() {};
    g_printLock.unlock();
    // Collect metrics
    bool didPass = true;
    int points_earned = 0;
    int points_possible = 0;
    int points_denominator = 0;
    for (int i = 0; i < test_runner_->tests_.size() - 1; i++) {
      UTTest* test = &test_runner_->tests_[i];
      if (!test->DidPass()) {
        didPass = false;
      }
      points_earned += test->PointsEarned();
      points_possible += test->points_possible;
      points_denominator += test->points_denominator;
    }
    *stream_ << std::endl;
    *stream_ << Blue << "Suite: " << suite_name_ << std::endl;
    *stream_ << Blue << "|" << std::endl;
    for (int i = 0; i < test_runner_->tests_.size() - 1; i++) {
      UTTest* test = &test_runner_->tests_[i];
      test->Print(*stream_);
    }
    if (didPass) {
      *stream_ << Green << "Passed!" << White << std::endl;
    } else {
      *stream_ << Red << "Failed!" << White << std::endl;
    }
    // Print report card
    if (points_possible > 0) {
      *stream_ << White << std::endl;
      *stream_ << Blue << "Points: ";
      int earned_percent_total = GetPercent(points_earned, points_denominator);
      PrintRatioColor(earned_percent_total);
      *stream_ << points_earned << "/" << points_denominator << ", "
               << earned_percent_total << "%" << std::endl;
      *stream_ << Blue << "|" << std::endl;
      for (int i = 0; i < test_runner_->tests_.size() - 1; i++) {
        UTTest& test = test_runner_->tests_[i];
        *stream_ << Blue << "|   ";
        int earned_percent =
            GetPercent(test.PointsEarned(), test.points_denominator);
        PrintRatioColor(earned_percent);
        *stream_ << std::setw(3) << std::right << test.PointsEarned()
                 << std::setw(3) << std::left << " / " << std::setw(3)
                 << std::left << test.points_denominator << std::setw(0)
                 << std::left << Blue << " - " << test.name << std::endl;
      }
      *stream_ << Blue << "|" << std::endl;
      *stream_ << Blue << "[";
      PrintRatioColor(earned_percent_total);
      for (int i = 0; i < 50; i++) {
        if (round(earned_percent_total / 2.0f) > i) {
          *stream_ << "=";
        } else {
          *stream_ << " ";
        }
      }
      if (points_possible > points_denominator) {
        *stream_ << Blue << "|" << Green;
        int extra_credit_points = points_possible - points_denominator;
        int extra_credit_percent =
            GetPercent(extra_credit_points, points_denominator);
        int earned_extra_credit_percent = earned_percent_total - 100;
        for (int i = 0; i < extra_credit_percent; i++) {
          if (earned_extra_credit_percent > i) {
            *stream_ << "=";
          } else {
            *stream_ << " ";
          }
        }
      }
      *stream_ << Blue << "]" << White << std::endl;
      *stream_ << std::endl;
    }
    std::cout.flush();

    // Write JSON if needed
    if (exporting_json_) {
      std::string json_string;
      try {
        json_string = test_runner_->WriteJson();
      } catch (...) {
        return;
      }
      std::cerr << "<JSON>" << json_string << "</JSON>" << std::endl;
    }
  };

  // Funally call both
  test_iterate_function_();
  test_end_function_();
}

void UTTestRunner::IsTrue(bool expression, std::string name,
                          std::string assertMessage) {
  // Add the condition
  active_test_->conditions.push_back(
      UTCondition(expression, name, assertMessage));
}

UTTestConfiguration* UTTestRunner::GetConfig() {
  return &(tests_[tests_.size() - 1]).configuration;
}

bool UTTestRunner::_TestPass(const std::string& testname, int points_possible,
                             int points_denominator) {
  return handler_(testname, points_possible, points_denominator);
}

void UTTestRunner::PrintRatioColor(int percent) {
  if (percent < 70) {
    *stream_ << Red;
  } else if (percent < 90) {
    *stream_ << Yellow;
  } else {
    *stream_ << Green;
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
  UTTest& test = tests_[tests_.size() - 1];
  test.name = test_name;
  test.points_possible = points_possible;
  test.points_denominator = points_denominator;

  // Create a new config for the next test
  tests_.push_back(UTTest());

  // Don't run the test
  return false;
}

bool UTTestRunner::RunHandler(const std::string& test_name, int points_possible,
                              int points_denominator) {
  // Returns true for ONLY the active test
  if (current_test_index_ == active_test_index_) {
    current_test_index_++;
    return true;
  }

  current_test_index_++;
  return false;
}

std::string UTTestRunner::WriteJson() {
  // Collect metrics
  bool did_pass = true;
  int points_earned = 0;
  int points_possible = 0;
  int points_denominator = 0;
  for (int i = 0; i < test_runner_->tests_.size() - 1; i++) {
    UTTest* test = &test_runner_->tests_[i];
    if (!test->DidPass()) {
      did_pass = false;
    }
    points_earned += test->PointsEarned();
    points_possible += test->points_possible;
    points_denominator += test->points_denominator;
  }
  ::nlohmann::json json = {{"total_points_earned", points_earned},
                           {"total_points_possible", points_possible},
                           {"total_points_denominator", points_denominator},
                           {"test_cases", ::nlohmann::json::array()}};
  for (int i = 0; i < test_runner_->tests_.size() - 1; i++) {
    json["test_cases"].push_back(
        ::nlohmann::json::parse(test_runner_->tests_[i].WriteJson()));
  }
  return json.dump();
}

void UTTestRunner::StdExceptionCatcher(
    std::function<void(UTTestRunner*)> suiteFunction) {
  try {
    suiteFunction(this);
  } catch (...) {
    active_test_->fatal_message = "A fatal exception was caught!";
  }
}

void UTTestRunner::MemoryMonitor(
    std::function<void(UTTestRunner*)> suiteFunction) {
  // Register Custom handlers for new / delete
  new_handler_ = [this](size_t size) -> void* {
    void* ptr = malloc(size);

    if (__line__ != -1) {
      this->active_test_->RegisterAllocation(
          MemoryAllocation(ptr, size, __file__, __line__));
    }

    return ptr;
  };

  delete_handler_ = [this](void* ptr) -> void {
    if (__line__ != -1) {
      this->active_test_->RegisterFree(ptr);
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
      *stream_ << Yellow
               << "One of the tests is taking an unusually long time...";
      warning_issued = true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Check if we timed out
  if (std::chrono::steady_clock::now() >= endTime) {
    workerThread.detach();
    active_test_->fatal_message =
        "Timed out, likley an infite loop or waiting for user input!";
  } else {
    workerThread.join();
  }

  std::cout.flush();
}

void SigSegVHandler(int signum) {
  *stream_ << "SIG-SEG Handler" << std::endl;

  // Re-Register the SIG-SEGV handler
  signal(SIGSEGV, SigSegVHandler);

  test_runner_->active_test_->fatal_message =
      "SEG-FAULT!\nYou are likley trying to dereference a null pointer.";
  test_iterate_function_();
  test_end_function_();
  std::cout.flush();
  signal(signum, SIG_DFL);
}

void UTTestRunner::SigSegCatcher(
    std::function<void(UTTestRunner*)> suiteFunction) {
  signal(SIGSEGV, SigSegVHandler);
  ThreadedTimeout(suiteFunction);
}
