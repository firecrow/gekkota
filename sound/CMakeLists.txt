add_library(sound OBJECT sound.c)
target_include_directories(sound PRIVATE ${CMAKE_SOURCE_DIR}/../../)