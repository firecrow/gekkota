add_library(engine OBJECT engine.cpp set_params.cpp)
target_include_directories(engine PRIVATE ${CMAKE_SOURCE_DIR}/../)
target_include_directories(engine PRIVATE ${CMAKE_SOURCE_DIR})