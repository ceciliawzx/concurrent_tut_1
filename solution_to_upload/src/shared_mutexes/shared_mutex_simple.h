#ifndef SHARED_MUTEX_SIMPLE_H
#define SHARED_MUTEX_SIMPLE_H

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "src/shared_mutexes/shared_mutex_base.h"

class SharedMutexSimple : public SharedMutexBase {
 public:
  SharedMutexSimple()
      : SharedMutexBase(), is_writer_(false), num_readers_(0u) {}

  void Lock() final {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(
        lock, [this]() -> bool { return !is_writer_ && num_readers_ == 0; });
    is_writer_ = true;
  }

  void Unlock() final {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(is_writer_);
    is_writer_ = false;
    condition_.notify_all();
  }

  void LockShared() final {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this]() -> bool { return !is_writer_; });
    num_readers_++;
  }

  void UnlockShared() final {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(num_readers_ > 0);
    num_readers_--;
    if (num_readers_ == 0) {
      condition_.notify_all();
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  bool is_writer_;
  size_t num_readers_;
};

#endif  // SHARED_MUTEX_SIMPLE_H
