cmake_minimum_required(VERSION 3.15)
project(TestSegPattern VERSION 0.1)
include(CTest)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_executable(TestSegPattern TestSegPattern.cpp)

add_subdirectory(../../segpattern sibling-segpattern)
add_subdirectory(../../block-sound-mem sibling-block-sound-mem)
target_link_libraries(TestSegPattern segpattern)
target_link_libraries(TestSegPattern block-sound-mem)
target_link_libraries(TestSegPattern gtest)
target_include_directories(TestSegPattern PRIVATE ${CMAKE_SOURCE_DIR}/../../)
target_include_directories(TestSegPattern PRIVATE ${CMAKE_SOURCE_DIR}/../../segpattern)
target_include_directories(TestSegPattern PRIVATE ${CMAKE_SOURCE_DIR}/../../block-sound-mem)

add_test(NAME TestSegPatternTest COMMAND TestSegPattern)