cmake_minimum_required(VERSION 3.15)
project(Gekkota VERSION 0.1)
include(CTest)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_executable(TestSegPattern TestSegPattern.cpp)

add_subdirectory(../../segpattern sibling-segpattern)
add_subdirectory(../../time sibling-time)
target_link_libraries(TestSegPattern segpattern)
target_link_libraries(TestSegPattern time)
target_link_libraries(TestSegPattern gtest)
target_include_directories(TestSegPattern PRIVATE ${CMAKE_SOURCE_DIR}/../../)
target_include_directories(TestSegPattern PRIVATE ${CMAKE_SOURCE_DIR}/../../segpattern)
target_include_directories(TestSegPattern PRIVATE ${CMAKE_SOURCE_DIR}/../../time)

add_test(NAME TestSegPatternTest COMMAND TestSegPattern)