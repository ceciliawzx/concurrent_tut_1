#ifndef RECURSIVE_MUTEX_BASE_H
#define RECURSIVE_MUTEX_BASE_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

class RecursiveMutex {
 public:
  void Lock() {
    std::unique_lock<std::mutex> lock(mutex_);

    condition_.wait(lock, [this]() -> bool {
      return lock_holder_id_ == std::thread::id() ||
             lock_holder_id_ == std::this_thread::get_id();
    });
    lock_holder_id_ = std::this_thread::get_id();
    lock_count_++;
  }

  void Unlock() {
    std::scoped_lock<std::mutex> lock(mutex_);
    assert(lock_holder_id_ == std::this_thread::get_id());
    assert(lock_count_ > 0);
    lock_count_--;
    if (lock_count_ == 0) {
      lock_holder_id_ = std::thread::id();
      condition_.notify_all();
    }
  }

 private:
  std::thread::id lock_holder_id_ = std::thread::id();
  int lock_count_ = 0;
  std::mutex mutex_;
  std::condition_variable condition_;
};

#endif  // RECURSIVE_MUTEX_BASE_H
