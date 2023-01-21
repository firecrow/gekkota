add_library(midi OBJECT MidiService.cpp accept.cpp)
target_include_directories(midi PRIVATE ${CMAKE_SOURCE_DIR})