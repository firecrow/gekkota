cmake_minimum_required(VERSION 3.15)
project(Gekkota VERSION 0.1)
include(CTest)

if("$ENV{GPU}" STREQUAL "hip")
    message("setting to gpu")
    set(CMAKE_C_COMPILER "/opt/rocm/bin/hipcc")
    set(CMAKE_CXX_COMPILER "/opt/rocm/bin/hipcc")
    add_compile_definitions(GPU_ARCH=hip)
elseif("$ENV{GPU}" STREQUAL "cuda")
    message("cuda not yet implemented, use `hip` instead")
else()
    message("setting to non-gpu")
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

message("using compilers C,C++ " ${CMAKE_C_COMPILER} "," ${CMAKE_CXX_COMPILER})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

message("Creating main executable variang: UI adds executable Gekkota")
add_executable(Gekkota ui/frontend.main.cpp)

add_subdirectory(sound)
target_link_libraries(Gekkota sound)

add_subdirectory(engine)
target_link_libraries(Gekkota engine)
if("$ENV{GPU}" STREQUAL "hip")
    target_include_directories(Gekkota PRIVATE "/opt/rocm/include")
    target_link_libraries(Gekkota engine-hip)
endif()

add_subdirectory(segpattern)
target_link_libraries(Gekkota segpattern)

add_subdirectory(instrument)
target_link_libraries(Gekkota instrument)

add_subdirectory(midi)
target_link_libraries(Gekkota midi)

add_subdirectory(frontend-service)
target_link_libraries(Gekkota frontend_service)

add_subdirectory(audio-output)
target_link_libraries(Gekkota audio_output)

add_subdirectory(time)
target_link_libraries(Gekkota time)

add_subdirectory(title)
target_link_libraries(Gekkota title)

add_subdirectory(logger)
target_link_libraries(Gekkota logger)

add_subdirectory(block-sound-mem)
target_link_libraries(Gekkota block-sound-mem)

add_subdirectory(gka-debug)
target_link_libraries(Gekkota gka-debug) 

target_link_libraries(Gekkota xkbcommon)
target_link_libraries(Gekkota /usr/local/share/static/libblend2d.a)

target_link_libraries(Gekkota asound)

target_include_directories(Gekkota PRIVATE ${CMAKE_SOURCE_DIR})

