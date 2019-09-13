
cmake_minimum_required(VERSION 3.10)
project(load_test)

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
