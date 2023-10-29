#include <chrono>
#include <vector>

#include "src/recursive_mutex/container.h"

int main() {
  Container<int> my_container;

  auto t1 = std::thread([&my_container]() -> void {
    std::vector<int> my_ints = {1, 2, 3, 4, 5, 6, 7};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    my_container.Add(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    my_container.AddAll(my_ints);
  });
  auto t2 = std::thread([&my_container]() -> void {
    std::vector<int> my_ints = {10, 20, 30, 40, 50, 60, 70};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    my_container.AddAll(my_ints);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    my_container.Add(100);
  });

  t1.join();
  t2.join();

  my_container.Show();
}
