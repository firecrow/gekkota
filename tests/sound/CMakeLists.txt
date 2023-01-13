cmake_minimum_required(VERSION 3.15)
project(Gekkota VERSION 0.1)
include(CTest)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_executable(TestSound TestSound.cpp)

add_subdirectory(../../sound sibling-sound)
add_subdirectory(../../segpattern sibling-segpattern)
add_subdirectory(../../time sibling-time)
target_link_libraries(TestSound segpattern)
target_link_libraries(TestSound time)
target_link_libraries(TestSound sound)
target_link_libraries(TestSound gtest)
target_include_directories(TestSound PRIVATE ${CMAKE_SOURCE_DIR}/../../)
target_include_directories(TestSound PRIVATE ${CMAKE_SOURCE_DIR}/../../segpattern)
target_include_directories(TestSound PRIVATE ${CMAKE_SOURCE_DIR}/../../time)
target_include_directories(TestSound PRIVATE ${CMAKE_SOURCE_DIR}/../../sound)

add_test(NAME TestSoundTest COMMAND TestSegPattern)