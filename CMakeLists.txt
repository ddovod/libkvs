
cmake_minimum_required(VERSION 3.10)
project(libkvs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

### index-cpp
add_library(index-cpp INTERFACE)
target_include_directories(index-cpp INTERFACE
  libs/index-cpp/src
  )

### json
add_library(json INTERFACE)
target_include_directories(json INTERFACE
  libs/json/include
  )

### teenypath
add_library(teenypath STATIC "")
target_sources(teenypath PRIVATE
  libs/teenypath/src/teenypath.cpp
  )
target_include_directories(teenypath PUBLIC
  libs/teenypath/include
  )

### libkvs
add_library(libkvs STATIC "")
target_sources(libkvs PRIVATE
  src/VirtualStorage.cpp

  src/linear_hashing/LinearHashVolume.cpp
  src/linear_hashing/LinearHashStorage.cpp
  src/linear_hashing/LinearHashStorageRegistry.cpp

  src/utility/TimeUtils.cpp
  src/utility/StringUtils.cpp
  src/utility/FilesystemUtils.cpp
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
target_link_libraries(libkvs index-cpp json teenypath)

### example app
add_subdirectory(example)

### tests
add_subdirectory(tests)
