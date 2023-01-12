#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <functional>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <vector>
#include <iostream>
#include <thread>
#include <inttypes.h>

#include <blend2d.h>

using namespace std;

typedef void (*void_fptr)();

#include "./math/utils_global.h"
#include "./frontend-service/frontend-service.h"
#include "./time/time.h"
#include "./logger/logger.h"
#include "./service/service.h"
#include "./engine/engine.h"
#include "./audio-output/audio.h"
#include "./segpattern/segpattern.h"
#include "./sound/sound.h"
#include "./midi/midi.h"
#include "./instrument/instrument.h"
#include "./instrument/dispatch.h"
#include "./instrument/tone.h"
#include "./ui/frontend.h"
#include "./title/title.h"
#include "./plot-visual/plot-visual.h"
