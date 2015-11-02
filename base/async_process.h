// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_ASYNC_PROCESS_H_
#define BASE_ASYNC_PROCESS_H_

#include <condition_variable>
#include <functional>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

#include "base/mutex.h"
#include "base/value_of.hh"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace base {

class AsyncProcess;
typedef std::shared_ptr<AsyncProcess> AsyncProcessPtr;

// No methods block
class AsyncProcess {
 public:
  AsyncProcess();

  // Both handlers will be called from the same thread, but not the calling
  // thread of FromForm. Output is guaranteed to be fully read before the
  // end_handler is called.
  static AsyncProcessPtr FromFork(
      std::function<int()> child_task,
      std::function<void(int, std::string)> output_handler,
      std::function<void(int)> end_handler);

  // See FromFork
  static AsyncProcessPtr FromExecv(
      const std::string& full_path, const std::vector<std::string> args,
      std::function<void(int, std::string)> output_handler,
      std::function<void(int)> end_handler);

  void Kill();

  bool DidExit();

 private:
  struct StreamToken {
    // See top of .cc for types
    int type;
    int exit_code;
    std::string content;
  };

  bool Execute(std::function<int()> child_task,
               std::function<void(int, std::string)> output_handler,
               std::function<void(int)> end_handler);

  // Marks a stream as closed (decrementing active_streams_ count). If both are
  // closed then it calls the end_handler_;
  void CloseStream();

  // Read lines from the give file handle and places them into the tokens_ queue
  // with the given channel.
  void ReadLinesFrom(int fd, int channel_id);

  std::mutex mutex_;
  int open_streams_ GUARDED_BY(mutex_);
  int pipes_[3][2] GUARDED_BY(mutex_);
  pid_t pid_ GUARDED_BY(mutex_);
  std::condition_variable cond_var_ GUARDED_BY(mutex_);
  std::vector<StreamToken> tokens_ GUARDED_BY(mutex_);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_ASYNC_PROCESS_H_
