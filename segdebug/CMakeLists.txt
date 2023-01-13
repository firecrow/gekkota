cmake_minimum_required(VERSION 3.15)
project(Gekkota-SegDebug VERSION 0.1)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

add_subdirectory(../plot-visual sibling-plot-visual)
add_subdirectory(../frontend-service sibling-frontend-service)
add_subdirectory(../math sibling-math)
add_subdirectory(../segpattern sibling-segpattern)

add_executable(SegDebug segdebug.cpp)
target_link_libraries(SegDebug plot_visual)
target_link_libraries(SegDebug math)
target_link_libraries(SegDebug segpattern)
target_link_libraries(SegDebug front_end_service)
target_link_libraries(SegDebug -lxkbcommon)
target_link_libraries(SegDebug /usr/local/share/static/libblend2d.a)