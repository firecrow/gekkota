add_library(engine OBJECT engine_utils.cpp set_params.cpp Engine.cpp HostRenderHandler.cpp)

if("$ENV{GPU}" STREQUAL "hip")
    add_library(engine-hip OBJECT HipDeviceRenderHandler.cpp)
    target_include_directories(engine-hip PRIVATE "/opt/rocm/include")
    target_include_directories(engine-hip PRIVATE ${CMAKE_SOURCE_DIR}/../)
    target_include_directories(engine-hip PRIVATE ${CMAKE_SOURCE_DIR})
endif()

target_include_directories(engine PRIVATE ${CMAKE_SOURCE_DIR}/../)
target_include_directories(engine PRIVATE ${CMAKE_SOURCE_DIR})