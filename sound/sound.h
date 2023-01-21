#include <math.h>
#include "segpattern/segpattern.h"

#define MAX_PHASE 2. * M_PI

gka_local_address_t gka_sound_create(
    struct gka_mem_block *blk, gka_local_address_t freq,
    gka_local_address_t volume
);

gka_local_address_t gka_sound_event_create(
    struct gka_mem_block *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
);

gka_local_address_t gka_sound_event_place(struct gka_mem_block *blk);

gka_decimal_t gka_get_frame_value_from_event(
    struct gka_mem_block *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const long rate
);