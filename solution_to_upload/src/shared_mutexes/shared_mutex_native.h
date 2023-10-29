#ifndef SHARED_MUTEX_NATIVE_H
#define SHARED_MUTEX_NATIVE_H

#include <shared_mutex>

#include "src/shared_mutexes/shared_mutex_base.h"

class SharedMutexNative : public SharedMutexBase {
 public:
  void Lock() final { mutex_.lock(); }

  void Unlock() final { mutex_.unlock(); }

  void LockShared() final { mutex_.lock_shared(); }

  void UnlockShared() final { mutex_.unlock_shared(); }

 private:
  std::shared_mutex mutex_;
};

#endif  // SHARED_MUTEX_NATIVE_H
