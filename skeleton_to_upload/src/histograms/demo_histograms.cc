#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>

static void RandomHistogramSC(std::array<std::atomic<int>, 10>& histogram,
                              size_t iterations) {
  // TODO
  // The following are to silence complaints about unused variables; remove
  // once you populate the function.
  (void)histogram;
  (void)iterations;
}

static void RandomHistogramRelaxed(std::array<std::atomic<int>, 10>& histogram,
                                   size_t iterations) {
  // TODO
  // The following are to silence complaints about unused variables; remove
  // once you populate the function.
  (void)histogram;
  (void)iterations;
}

int main() {
  // Example of how to declare a Mersenne Twister 19937 engine and use it to
  // generate a random integer.
  std::random_device device;

  // Replace device() with a fixed integer if you would like to make the
  // numbers that are generated deterministic.
  std::mt19937 generator(device());
  std::uniform_int_distribution<int> distribution(0, 9);
  std::cout << distribution(generator) << std::endl;

  // Example benchmarking code.
  auto begin_time = std::chrono::high_resolution_clock::now();

  // The code you wish to benchmark.

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = end_time - begin_time;
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  std::cout << "Duration: " << millis << " ms" << std::endl;

  // The following are to silence complaints about unused functions; remove
  // in due course.
  (void)RandomHistogramSC;
  (void)RandomHistogramRelaxed;

  return 0;
}
