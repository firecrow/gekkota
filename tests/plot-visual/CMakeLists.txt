cmake_minimum_required(VERSION 3.15)
project(Gekkota VERSION 0.1)
include(CTest)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)


add_subdirectory(../../plot-visual sibling-plot-visual)
add_subdirectory(../../math sibling-math)

add_executable(TestPlot TestGkaPlot.cpp)

target_link_libraries(TestPlot math)
target_link_libraries(TestPlot /usr/local/share/static/libblend2d.a gtest)
target_include_directories(TestPlot PRIVATE ${CMAKE_SOURCE_DIR}/../../plot-visual)
target_link_libraries(TestPlot plot_visual)

add_test(NAME TestPlotTest COMMAND TestPlot)