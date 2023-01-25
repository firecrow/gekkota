add_library(audio-segment OBJECT seg-management.c step-calculation.c phase-calculation.c process-frame.c)
target_include_directories(audio-segment PRIVATE ${CMAKE_SOURCE_DIR})