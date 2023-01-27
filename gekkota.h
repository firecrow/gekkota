#include "gekkota-external.h"

using namespace std;

typedef void (*void_fptr)();

#include "./audio-segment/audio-segment.h"

extern "C" {
#include "./time/gka_time.h"
#include "./gka-debug/gka_debug.h"
}

#include "./math/utils_global.h"
#include "./frontend-service/frontend-service.h"
#include "./logger/logger.h"
#include "./service/service.h"
#include "./engine/engine.h"
#include "./audio-output/audio.h"
#include "./midi/midi.h"
#include "./instrument/instrument.h"
#include "./instrument/dispatch.h"
#include "./instrument/tone.h"
#include "./ui/frontend.h"
#include "./title/title.h"
#include "./plot-visual/plot-visual.h"
