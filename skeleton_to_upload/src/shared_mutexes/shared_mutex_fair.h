#ifndef SHARED_MUTEX_FAIR_H
#define SHARED_MUTEX_FAIR_H

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "src/shared_mutexes/shared_mutex_base.h"

class SharedMutexFair : public SharedMutexBase {
 public:
  void Lock() final {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this]() -> bool { return !writer_active_; });
    writer_active_ = true;
    cond_var_.wait(
        lock, [this]() -> bool { return read_acquires_ == read_releases_; });
  }

  void Unlock() final {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(writer_active_);
    writer_active_ = false;
    cond_var_.notify_all();
  }

  void LockShared() final {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this]() -> bool { return !writer_active_; });
    read_acquires_++;
  }

  void UnlockShared() final {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(read_acquires_ > read_releases_);
    read_releases_++;
    if (read_acquires_ == read_releases_) {
      cond_var_.notify_all();
    }
  }

 private:
  int read_acquires_ = 0;
  int read_releases_ = 0;
  bool writer_active_ = false;
  std::mutex mutex_;
  std::condition_variable cond_var_;
};

#endif  // SHARED_MUTEX_FAIR_H
