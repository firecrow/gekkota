#include "audio-segment.h"
#include <hip/hip_runtime.h>

__PROCESS_GPU__ gka_decimal_t gka_get_frame_value_from_event_hipdevice(
    struct gka_entry *blk, double *phases, struct gka_entry *event, int soundId,
    gka_time_t start, int frame, gka_time_t local, const uint32_t rate,
    int period_size
);

__PROCESS_GPU__ double gka_get_frame_step_from_event_hipdevice(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start, int frame,
    gka_time_t local, const uint32_t rate
);

__PROCESS_GPU__ gka_decimal_t gka_frame_from_block_hipdevice(
    struct gka_entry *blk, double *phases, int frame, gka_time_t local,
    int rate, int period_size
);

__PROCESS_GPU__ void gka_set_steps_from_hipdevice(
    struct gka_entry *blk, double *out, int frame, gka_time_t local, int rate,
    int period_size
);

__PROCESS_GPU__ void gka_set_phases_for_event_hipdevice(
    double *dest, double *steps, int soundId, int period_size
);
