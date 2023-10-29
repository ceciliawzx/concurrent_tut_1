#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "src/shared_mutexes/shared_mutex_base.h"
#include "src/shared_mutexes/shared_mutex_fair.h"
#include "src/shared_mutexes/shared_mutex_native.h"
#include "src/shared_mutexes/shared_mutex_simple.h"
#include "src/shared_mutexes/shared_mutex_stupid.h"

static void Writer(int& shared_int, int max_value,
                   SharedMutexBase& shared_mutex) {
  for (int i = 0; i < max_value; i++) {
    shared_mutex.Lock();
    shared_int++;
    shared_mutex.Unlock();
  }
}

static void DoWork() {
  for (volatile int i = 0; i < 10000; i++) {
  }
}
static void Reader(const int& shared_int, int max_value,
                   SharedMutexBase& shared_mutex,
                   std::atomic<size_t>& total_read_attempts,
                   size_t max_read_attempts) {
  bool done = false;
  size_t my_read_attempts = 0u;
  while (!done) {
    my_read_attempts++;
    shared_mutex.LockShared();
    DoWork();
    if (shared_int >= max_value || my_read_attempts >= max_read_attempts) {
      done = true;
    }
    shared_mutex.UnlockShared();
  }
  total_read_attempts.fetch_add(my_read_attempts);
}

static void RunBenchmark(const std::string& benchmark_name,
                         SharedMutexBase& shared_mutex, int max_value,
                         size_t num_readers, size_t max_read_attempts) {
  std::cout << "Running " << benchmark_name << std::endl;
  auto begin_time = std::chrono::high_resolution_clock::now();
  int shared_int = 0;
  std::atomic<size_t> total_read_attempts(0u);
  std::vector<std::thread> readers;
  for (size_t i = 0; i < num_readers; i++) {
    readers.emplace_back(std::thread(
        Reader, std::ref(shared_int), max_value, std::ref(shared_mutex),
        std::ref(total_read_attempts), max_read_attempts));
  }
  auto writer = std::thread(Writer, std::ref(shared_int), max_value,
                            std::ref(shared_mutex));
  for (auto& reader : readers) {
    reader.join();
  }
  writer.join();
  auto end_time = std::chrono::high_resolution_clock::now();
  assert(shared_int == max_value);
  auto duration = end_time - begin_time;
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  std::cout << "Duration: " << millis << " ms" << std::endl;
  std::cout << "Total read attempts: " << total_read_attempts << std::endl;
}

int main() {
  {
    SharedMutexStupid shared_mutex_stupid;
    RunBenchmark("Stupid", shared_mutex_stupid, 100, 8u, 100000u);
  }
  {
    SharedMutexSimple shared_mutex_simple;
    RunBenchmark("Simple", shared_mutex_simple, 100, 8u, 100000u);
  }
  {
    SharedMutexFair shared_mutex_fair;
    RunBenchmark("Fair", shared_mutex_fair, 100, 8u, 100000u);
  }
  {
    SharedMutexNative shared_mutex_native;
    RunBenchmark("Native", shared_mutex_native, 100, 8u, 100000u);
  }
  return 0;
}
