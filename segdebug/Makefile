C++ = clang++
CFLAGS ?= -g -std=c++11
OBJECTS = ../build/objects/GkaPlot.o ../build/objects/GkaCrosshair.o ../build/objects/GkaChart.o ../build/objects/GkaChartDetail.o
OUTDIR = ../bin
TARGET_NAME = segdebug
TARGET = $(OUTDIR)/$(TARGET_NAME)

LIBS = /usr/local/share/static/libblend2d.a ../utils_global.cpp 

WAYLAND_FLAGS = $(shell pkg-config wayland-client --cflags --libs)
WAYLAND_PROTOCOLS_DIR = $(shell pkg-config wayland-protocols --variable=pkgdatadir)
WAYLAND_SCANNER = $(shell pkg-config --variable=wayland_scanner wayland-scanner)
FE_LIBS =  -lrt -lxkbcommon -lm -lasound $(WAYLAND_FLAGS)

SOURCES = ./segdebug.cpp
SEGPATTERN_SOURCES = ../segpattern/segpattern.cpp

TIME_SOURCES = ./time/time_utils.cpp
FRONTEND_DEP_SOURCES =  ../ui/xdg-shell-protocol.c ../ui/shm.c
FRONTEND_SOURCES = ../ui/FrontEndService.cpp $(FRONTEND_DEP_SOURCES) 

HEADERS = ./segdebug.h

all: segdebug

segdebug: $(OBJECTS) $(SOURCES) $(HEADERS) $(FRONTEND_SOURCES)
	$(C++) -o $(TARGET) $(SOURCES) $(SEGPATTERN_SOURCES) $(OBJECTS) $(FRONTEND_SOURCES) $(LIBS) $(FE_LIBS)