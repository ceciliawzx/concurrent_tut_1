#ifndef SHARED_MUTEX_BASE_H
#define SHARED_MUTEX_BASE_H

class SharedMutexBase {
 public:
  virtual ~SharedMutexBase() = default;

  // Obtain the writer lock
  virtual void Lock() = 0;

  // Release the writer lock
  virtual void Unlock() = 0;

  // Obtain a reader lock
  virtual void LockShared() = 0;

  // Release a reader lock
  virtual void UnlockShared() = 0;
};

#endif  // SHARED_MUTEX_BASE_H
