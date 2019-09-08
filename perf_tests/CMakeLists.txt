
cmake_minimum_required(VERSION 3.10)
project(perf_tests)

add_executable(kvs-perf-tests "")
target_sources(kvs-perf-tests PRIVATE
  src/main.cpp
  src/ReadWritePerfBasic_test.cpp
  )
target_include_directories(kvs-perf-tests PRIVATE
  src
  ../libs/doctest
  )
target_link_libraries(kvs-perf-tests PRIVATE
  libkvs
  )
set_target_properties(kvs-perf-tests PROPERTIES
  CXX_STANDARD 17
  )
