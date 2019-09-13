
cmake_minimum_required(VERSION 3.10)
project(load_test)

option(KVS_TESTS_ASAN "Enable address sanitizer for tests" OFF)
option(KVS_TESTS_TSAN "Enable thread sanitizer for tests" OFF)

### argh
add_library(argh INTERFACE)
target_include_directories(argh INTERFACE
  ../libs/argh/include
  )

### backward-cpp
add_library(backward-cpp STATIC "")
target_sources(backward-cpp PRIVATE
  ../libs/backward-cpp/backward.cpp
  )
target_include_directories(backward-cpp PUBLIC
  ../libs/backward-cpp
  )

### kvs-load-test
add_executable(kvs-load-test "")
target_sources(kvs-load-test PRIVATE
  src/main.cpp
  src/TestRunner.cpp
  src/utility.cpp
  )
target_include_directories(kvs-load-test PRIVATE
  src
  )
find_package(Threads REQUIRED)
target_link_libraries(kvs-load-test PRIVATE
  libkvs
  argh
  backward-cpp
  Threads::Threads
  bfd
  dl
  )
set_target_properties(kvs-load-test PROPERTIES
  CXX_STANDARD 17
  )

if (KVS_TESTS_ASAN AND KVS_TESTS_TSAN)
  message(FATAL_ERROR "Cannot enable both asan and tsan, please choose only one")
endif()

if (KVS_TESTS_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer ")
endif()

if (KVS_TESTS_TSAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread ")
endif()
