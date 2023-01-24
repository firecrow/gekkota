#ifndef __GKA_SOUND_HEADER
#define __GKA_SOUND_HEADER

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "time/gka_time.h"
#include "segpattern/segpattern.h"

#define MAX_PHASE 2. * M_PI

gka_local_address_t gka_sound_create(
    struct gka_entry *blk, gka_local_address_t freq, gka_local_address_t volume
);

gka_local_address_t gka_sound_event_create(
    struct gka_entry *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
);

gka_local_address_t gka_sound_event_place(struct gka_entry *blk);

gka_decimal_t gka_get_frame_value_from_event(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const uint32_t rate
);

gka_decimal_t
gka_frame_from_block(struct gka_entry *blk, gka_time_t local, int rate);

#endif