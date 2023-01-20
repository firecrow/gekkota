add_library(sound OBJECT sound.c)
target_include_directories(sound PRIVATE ${CMAKE_SOURCE_DIR}/../../block-sound-mem)
target_include_directories(sound PRIVATE ${CMAKE_SOURCE_DIR}/../../segpattern)