add_library(audio-segment OBJECT host-calculations.c math-transitions.c retrieve-seg.c seg-management.c)
target_include_directories(audio-segment PRIVATE ${CMAKE_SOURCE_DIR})

if("$ENV{GPU}" STREQUAL "hip")
    add_library(audio-segment-hip OBJECT hip-calculations.c)
    target_include_directories(audio-segment-hip PRIVATE ${CMAKE_SOURCE_DIR})
endif()