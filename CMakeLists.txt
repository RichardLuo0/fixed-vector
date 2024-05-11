cmake_minimum_required(VERSION 3.28)
project(fixed-vector CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_compile_options(-Wall)

# std module
option(stdPath "The path to std module, for example: llvm-project/build/modules/c++/v1")

include(FetchContent)
FetchContent_Declare(
  std
  URL "${stdPath}"
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SYSTEM
)
FetchContent_MakeAvailable(std)

# Build lib
add_library(fixedVector)
file(GLOB_RECURSE SRC_FILES "src/*.cppm")
target_sources(fixedVector
  PUBLIC FILE_SET CXX_MODULES
  FILES ${SRC_FILES}
)
target_link_libraries(fixedVector PRIVATE std)

# Build exe
add_executable(test tests/main.cpp)
target_link_libraries(test PRIVATE fixedVector)
target_link_libraries(test PRIVATE std)
