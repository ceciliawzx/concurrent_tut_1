cmake_minimum_required(VERSION 3.13)

project(hashsets
    VERSION 1.0
    DESCRIPTION "C++ Concurrency Tutorial"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")

find_package(Threads REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
  add_compile_options(-Werror -Wall -Wextra -pedantic -Weverything)
  add_compile_options(
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Wno-padded
          -Wno-weak-vtables
  )
  set(USE_SANITIZER "" CACHE STRING
          "Use the clang sanitizer [asan|tsan]")
  if("${USE_SANITIZER}" STREQUAL "asan")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope)
    add_link_options(-fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope)
  elseif("${USE_SANITIZER}" STREQUAL "tsan")
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
  elseif(NOT "${USE_SANITIZER}" STREQUAL "")
    message(FATAL_ERROR "Unknown argument to USE_SANITIZER: ${USE_SANITIZER} - options are [asan|tsan]")
  endif()
else()
  message(FATAL_ERROR "The clang compiler is required")
endif()

add_executable(demo_recursive_mutexes
        src/recursive_mutex/container.h
        src/recursive_mutex/recursive_mutex.h
        src/recursive_mutex/demo_recursive_mutexes.cc
        )
target_include_directories(demo_recursive_mutexes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(demo_recursive_mutexes PRIVATE Threads::Threads)

add_executable(demo_shared_mutexes
  src/shared_mutexes/shared_mutex_base.h
  src/shared_mutexes/shared_mutex_fair.h
  src/shared_mutexes/shared_mutex_simple.h
  src/shared_mutexes/shared_mutex_stupid.h
  src/shared_mutexes/demo_shared_mutexes.cc
  )
target_include_directories(demo_shared_mutexes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(demo_shared_mutexes PRIVATE Threads::Threads)

add_executable(demo_random_sets
        src/random_sets/demo_random_sets.cc
        )
target_link_libraries(demo_random_sets PRIVATE Threads::Threads)

add_executable(demo_histograms
        src/histograms/demo_histograms.cc
        )
target_link_libraries(demo_histograms PRIVATE Threads::Threads)
