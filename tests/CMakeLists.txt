
cmake_minimum_required(VERSION 3.10)
project(tests)

option(KVS_TESTS_ASAN "Enable address sanitizer for tests" OFF)
option(KVS_TESTS_TSAN "Enable thread sanitizer for tests" OFF)

add_executable(kvs-tests "")
target_sources(kvs-tests PRIVATE
  src/main.cpp
  src/BasicOperations_test.cpp
  src/LinearHashStorageRegistry_test.cpp
  src/MountingNodes_test.cpp
  src/UnmountingNodes_test.cpp
  # src/SimpleMultiThreaded_test.cpp
  src/KeyRange_test.cpp
  )
target_include_directories(kvs-tests PRIVATE
  src
  ../libs/doctest
  )
find_package(Threads REQUIRED)
target_link_libraries(kvs-tests PRIVATE
  libkvs
  Threads::Threads
  )
set_target_properties(kvs-tests PROPERTIES
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
