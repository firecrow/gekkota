add_library(audio_output OBJECT AudioOutputService.cpp)
target_include_directories(audio_output PRIVATE ${CMAKE_SOURCE_DIR})