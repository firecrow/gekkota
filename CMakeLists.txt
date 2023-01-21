cmake_minimum_required(VERSION 3.15)
project(Gekkota VERSION 0.1)
include(CTest)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

message("Creating main executable variang: UI adds executable Gekkota")
add_executable(Gekkota ui/frontend.main.cpp)

add_subdirectory(engine)
target_link_libraries(Gekkota engine)

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

add_subdirectory(sound)
target_link_libraries(Gekkota sound)

add_subdirectory(time)
target_link_libraries(Gekkota time)

add_subdirectory(title)
target_link_libraries(Gekkota title)

add_subdirectory(logger)
target_link_libraries(Gekkota logger)

target_link_libraries(Gekkota xkbcommon)
target_link_libraries(Gekkota /usr/local/share/static/libblend2d.a)
target_link_libraries(Gekkota asound)

target_include_directories(Gekkota PRIVATE ${CMAKE_SOURCE_DIR}/../)
target_include_directories(Gekkota PRIVATE ${CMAKE_SOURCE_DIR}/../segpattern)
target_include_directories(Gekkota PRIVATE ${CMAKE_SOURCE_DIR}/../block-sound-mem)
target_include_directories(Gekkota PRIVATE ${CMAKE_SOURCE_DIR}/../time)
target_include_directories(Gekkota PRIVATE ${CMAKE_SOURCE_DIR}/../sound)
