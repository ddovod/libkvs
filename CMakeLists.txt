
cmake_minimum_required(VERSION 3.10)
project(libkvs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

### libkvs
add_library(libkvs STATIC "")
target_sources(libkvs PRIVATE
  src/IStorage.cpp
  src/VirtualStorage.cpp
  
  src/utility/TimeUtils.cpp
  )
target_include_directories(libkvs PUBLIC
  src
  )
set_target_properties(libkvs PROPERTIES
  CXX_STANDARD 17
  )
target_compile_options(libkvs PRIVATE
  -fno-rtti
  -fcolor-diagnostics
  -fno-ident
  -fno-stack-protector

# Warnings
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wconversion
  -Wvexing-parse
  -Werror=return-type
  )

### example app
add_subdirectory(example)

### tests
add_subdirectory(tests)
