#include "audio-segment.h"

gka_decimal_t gka_get_frame_value_from_event(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const uint32_t rate
);

gka_decimal_t
gka_frame_from_block(struct gka_entry *blk, gka_time_t local, int rate);