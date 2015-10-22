// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_BLOCKING_STREAM_HH_
#define BASE_BLOCKING_STREAM_HH_

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "base/log.h"
#include "base/mutex.h"
#include "base/value_of.hh"

namespace thilenius {
namespace base {

template <typename T>
class BlockingStream {
 public:
  BlockingStream() : stream_ended(false) {}

  // Starts a thread to pull items from a source. Can have multiple sources
  void SourceFrom(const std::function<ValueOf<T>()>& source) {
    std::thread([this, source]() {
      while (!stream_ended) {
        ValueOf<T> value = source();
        if (!value.IsValid()) {
          return;
        } else {
          Write(value.GetOrDie());
        }
      }
    }).detach();
  }

  // Stars a thread and end the stream once blocking_source returns
  void EndFrom(const std::function<void()>& blocking_source) {
    std::thread([this, blocking_source]() {
      blocking_source();
      WriteEndOfStream();
    }).detach();
  }

  // Writes item to the stream
  void Write(const T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    data_.push_back(item);
    cond_var_.notify_all();
  }

  // Writes items to the stream
  void Write(const std::vector<T>& items) {
    std::unique_lock<std::mutex> lock(mutex_);
    data_.reserve(items.size());
    data_.insert(data_.end(), items.begin(), items.end());
    cond_var_.notify_all();
  }

  // Writes an end of stream
  void WriteEndOfStream() {
    std::unique_lock<std::mutex> lock(mutex_);
    stream_ended = true;
    cond_var_.notify_all();
  }

  // Tries to immediately return 1 or more item. If itdex == size then
  // ReadItemsAfterIndex will block until at least 1 item is present.
  ValueOf<std::vector<T>> ReadItemsAfterIndex(int index) {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!stream_ended && index >= data_.size()) {
      cond_var_.wait(lock);
    }
    // Stream ended, it will never have items past size() - 1
    if (stream_ended && index >= data_.size()) {
      return {std::vector<T>(), "End of stream"};
    }
    std::vector<T> items;
    for (int i = index; i < data_.size(); i++) {
      items.push_back(data_[i]);
    }
    return std::move(items);
  }

 private:
  std::mutex mutex_;
  std::condition_variable cond_var_ GUARDED_BY(mutex_);
  std::vector<T> data_ GUARDED_BY(mutex_);
  bool stream_ended GUARDED_BY(mutex_);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_BLOCKING_STREAM_HH_
