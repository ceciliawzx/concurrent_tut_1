#include <array>
#include <atomic>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

static const size_t kMaxValue = 1 << 24;

static void RandomSetSC(std::array<std::atomic<bool>, kMaxValue>& random_set,
                        size_t iterations) {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<size_t> distribution(0, kMaxValue);
  for (size_t i = 0; i < iterations; i++) {
    random_set[distribution(generator)].store(true);
  }
}

static void RandomSetRelaxed(
    std::array<std::atomic<bool>, kMaxValue>& random_set, size_t iterations) {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<size_t> distribution(0, kMaxValue);
  for (size_t i = 0; i < iterations; i++) {
    random_set[distribution(generator)].store(true, std::memory_order_relaxed);
  }
}

static void RunBenchmark(
    const std::string& name,
    const std::function<void(std::array<std::atomic<bool>, kMaxValue>&,
                             size_t)>& thread_body) {
  std::cout << "Running " << name << std::endl;
  auto set = std::make_unique<std::array<std::atomic<bool>, kMaxValue>>();
  for (size_t i = 0; i < kMaxValue; i++) {
    (*set)[i] = false;
  }
  auto begin_time = std::chrono::high_resolution_clock::now();
  std::vector<std::thread> threads;
  for (size_t i = 0; i < 8; i++) {
    threads.emplace_back(std::thread(thread_body, std::ref(*set), 1 << 24));
  }
  for (auto& thread : threads) {
    thread.join();
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = end_time - begin_time;
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  std::cout << "Duration: " << millis << " ms" << std::endl;
  size_t num_set_elements = 0;
  for (size_t i = 0; i < kMaxValue; i++) {
    if ((*set)[i]) {
      num_set_elements++;
    }
  }
  std::cout << "Size of set: " << num_set_elements << std::endl;
}

int main() {
  RunBenchmark("Sequentially consistent", RandomSetSC);
  RunBenchmark("Relaxed", RandomSetRelaxed);
  return 0;
}
