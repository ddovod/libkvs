
cmake_minimum_required(VERSION 3.10)
project(example)

add_executable(kvs-example "")
target_sources(kvs-example PRIVATE
  src/main.cpp
  )
target_include_directories(kvs-example PRIVATE
  src
  )
target_link_libraries(kvs-example PRIVATE
  libkvs
  )
