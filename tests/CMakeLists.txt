
cmake_minimum_required(VERSION 3.10)
project(tests)

add_executable(kvs-tests "")
target_sources(kvs-tests PRIVATE
  src/main.cpp
  src/BasicOperations_test.cpp
  )
target_include_directories(kvs-tests PRIVATE
  src
  ../libs/doctest
  )
target_link_libraries(kvs-tests PRIVATE
  libkvs
  )
set_target_properties(kvs-tests PROPERTIES
  CXX_STANDARD 17
  )
