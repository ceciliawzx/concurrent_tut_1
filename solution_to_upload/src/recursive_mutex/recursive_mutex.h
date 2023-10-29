#ifndef RECURSIVE_MUTEX_BASE_H
#define RECURSIVE_MUTEX_BASE_H

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class RecursiveMutex {
 public:
  RecursiveMutex() : num_holds_(0) {}

  // Obtain the lock
  void Lock() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this]() -> bool {
      return owner_ == std::thread::id() ||
             owner_ == std::this_thread::get_id();
    });
    owner_ = std::this_thread::get_id();
    num_holds_++;
  }

  // Release the lock
  void Unlock() {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(owner_ == std::this_thread::get_id());
    assert(num_holds_ > 0);
    num_holds_--;
    if (num_holds_ == 0) {
      owner_ = std::thread::id();
      condition_.notify_all();
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  std::thread::id owner_;
  size_t num_holds_;
};

#endif  // RECURSIVE_MUTEX_BASE_H
