#ifndef SHARED_MUTEX_SIMPLE_H
#define SHARED_MUTEX_SIMPLE_H

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "src/shared_mutexes/shared_mutex_base.h"

class SharedMutexSimple : public SharedMutexBase {
 public:
  void Lock() final {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this]() -> bool {
      return !writer_active_ && active_readers_ == 0;
    });
    writer_active_ = true;
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
    active_readers_++;
  }

  void UnlockShared() final {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(active_readers_ > 0);
    active_readers_--;
    if (active_readers_ == 0) {
      cond_var_.notify_all();
    }
  }

 private:
  int active_readers_ = 0;
  bool writer_active_ = false;
  std::mutex mutex_;
  std::condition_variable cond_var_;
};

#endif  // SHARED_MUTEX_SIMPLE_H
