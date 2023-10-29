#include <array>
#include <atomic>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

static void RandomHistogramSC(std::array<std::atomic<int>, 10>& histogram,
                              size_t iterations) {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<int> distribution(0, 9);
  for (size_t i = 0; i < iterations; i++) {
    histogram[static_cast<size_t>(distribution(generator))].fetch_add(1);
  }
}

static void RandomHistogramRelaxed(std::array<std::atomic<int>, 10>& histogram,
                                   size_t iterations) {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<int> distribution(0, 9);
  for (size_t i = 0; i < iterations; i++) {
    histogram[static_cast<size_t>(distribution(generator))].fetch_add(
        1, std::memory_order_relaxed);
  }
}

static void RandomHistogramLocal(std::array<std::atomic<int>, 10>& histogram,
                                 size_t iterations) {
  std::array<int, 10> my_histogram{};
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_int_distribution<int> distribution(0, 9);
  for (size_t i = 0; i < iterations; i++) {
    my_histogram[static_cast<size_t>(distribution(generator))]++;
  }
  for (size_t i = 0; i < 10; i++) {
    histogram[i].fetch_add(my_histogram[i], std::memory_order_relaxed);
  }
}

static void RunBenchmark(
    const std::string& name,
    const std::function<void(std::array<std::atomic<int>, 10>&, size_t)>&
        thread_body) {
  std::cout << "Running " << name << std::endl;
  auto begin_time = std::chrono::high_resolution_clock::now();
  std::array<std::atomic<int>, 10> histogram{};
  std::vector<std::thread> threads;
  for (size_t i = 0; i < 8; i++) {
    threads.emplace_back(
        std::thread(thread_body, std::ref(histogram), 1 << 24));
  }
  for (auto& thread : threads) {
    thread.join();
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = end_time - begin_time;
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  std::cout << "Duration: " << millis << " ms" << std::endl;
  for (size_t i = 0; i < 10; i++) {
    std::cout << i << ": " << histogram[i] << std::endl;
  }
}

int main() {
  RunBenchmark("Sequentially consistent", RandomHistogramSC);
  RunBenchmark("Relaxed", RandomHistogramRelaxed);
  RunBenchmark("Local", RandomHistogramLocal);
  return 0;
}
