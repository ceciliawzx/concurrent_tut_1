#ifndef SHARED_MUTEX_FAIR_H
#define SHARED_MUTEX_FAIR_H

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "src/shared_mutexes/shared_mutex_base.h"

class SharedMutexFair : public SharedMutexBase {
 public:
  SharedMutexFair()
      : SharedMutexBase(),
        is_writer_(false),
        read_acquires_(0u),
        read_releases_(0u),
        ghost_num_readers_(0u) {}

  void Lock() final {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this]() -> bool { return !is_writer_; });
    is_writer_ = true;
    condition_.wait(
        lock, [this]() -> bool { return read_acquires_ == read_releases_; });
    assert(ghost_num_readers_ == 0);
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
    assert(Invariant());
    read_acquires_++;
    ghost_num_readers_++;
    assert(Invariant());
  }

  void UnlockShared() final {
    std::unique_lock<std::mutex> lock(mutex_);
    assert(read_acquires_ > read_releases_);
    assert(Invariant());
    read_releases_++;
    ghost_num_readers_--;
    assert(Invariant());
    if (read_acquires_ == read_releases_) {
      assert(ghost_num_readers_ == 0);
      condition_.notify_all();
    }
  }

 private:
  // Check that the ghost field is always related to the pair of fields.
  [[nodiscard]] bool Invariant() const {
    return ghost_num_readers_ == read_acquires_ - read_releases_;
  }

  std::mutex mutex_;
  std::condition_variable condition_;
  bool is_writer_;
  size_t read_acquires_;
  size_t read_releases_;

  // A ghost field, intended to demonstrate that there is no need for the
  // pair of fields, read_acquires_ and read_releases_
  size_t ghost_num_readers_;
};

#endif  // SHARED_MUTEX_FAIR_H
