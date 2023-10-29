#ifndef CONTAINER_H
#define CONTAINER_H


#include <iostream>
#include <vector>
#include "recursive_mutex.h"

template <typename T>
class Container {
 public:

  Container(): mutex_(RecursiveMutex()) {}

  // ? if Add doesn't require lock here, how does it ensure synchronization?
  void Add(const T& elem) {
    mutex_.Lock();
    contents_.push_back(elem);
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
    for (auto& elem : contents_) {
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
  std::vector<T> contents_;
  RecursiveMutex mutex_;
};

#endif  // CONTAINER_H
