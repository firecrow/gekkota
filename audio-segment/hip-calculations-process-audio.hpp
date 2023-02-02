#include "audio-segment.h"

/**************************************************************************
 * Main function interface into the GPU processing code
 **************************************************************************/
__PROCESS_HOST__ void gka_process_audio_hip(
    double *dest, struct gka_entry *src, int count, int rate, gka_time_t elapsed
);