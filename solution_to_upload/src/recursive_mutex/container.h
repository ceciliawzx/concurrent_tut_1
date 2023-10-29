#ifndef CONTAINER_H
#define CONTAINER_H

#include <iostream>
#include <vector>

#include "src/recursive_mutex/recursive_mutex.h"

template <typename T>
class Container {
 public:
  void Add(const T& elem) {
    mutex_.Lock();
    data_.push_back(elem);
    mutex_.Unlock();
  }

  void AddAll(const std::vector<T>& elems) {
    mutex_.Lock();
    for (auto& elem : elems) {
      Add(elem);
    }
    mutex_.Unlock();
  }

  void Show() {
    mutex_.Lock();
    std::cout << "[";
    bool first = true;
    for (auto& elem : data_) {
      if (!first) {
        std::cout << ", ";
      }
      first = false;
      std::cout << elem;
    }
    std::cout << "]" << std::endl;
    mutex_.Unlock();
  }

 private:
  RecursiveMutex mutex_;
  std::vector<T> data_;
};

#endif  // CONTAINER_H
