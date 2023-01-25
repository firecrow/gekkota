add_library(audio-segment OBJECT host-calculations.cpp math-transitions.cpp retrieve-seg.cpp seg-management.cpp)
target_include_directories(audio-segment PRIVATE ${CMAKE_SOURCE_DIR})

if("$ENV{GPU}" STREQUAL "hip")
    add_library(audio-segment-hip OBJECT hip-calculations.cpp)
    target_include_directories(audio-segment-hip PRIVATE ${CMAKE_SOURCE_DIR})
endif()