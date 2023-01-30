
if("$ENV{GPU}" STREQUAL "hip")
    add_library(audio-segment-hip OBJECT hip-calculations.cpp)
    target_include_directories(audio-segment-hip PRIVATE ${CMAKE_SOURCE_DIR})

    add_library(audio-segment OBJECT host-calculations.cpp math-transitions.c retrieve-seg.c seg-management.c)
    target_include_directories(audio-segment PRIVATE ${CMAKE_SOURCE_DIR})
else()
    add_library(audio-segment OBJECT host-calculations.cpp math-transitions.c retrieve-seg.c seg-management.c)
    target_include_directories(audio-segment PRIVATE ${CMAKE_SOURCE_DIR})
endif()