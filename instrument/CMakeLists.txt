add_library(instrument OBJECT Classical.cpp dispatch.cpp Instrument.cpp InstrumentRouter.cpp KeysInstrument.cpp Tone.cpp)
target_include_directories(instrument PRIVATE ${CMAKE_SOURCE_DIR})