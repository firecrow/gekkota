#include "audio-segment.h"
#include "seg-management.c"
#include "retrieve-seg.c"
#include "math-transitions.c"

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

__PROCESS_BRIDGE__ void gkaHipSetSteps(
    gka_decimal_t *dest, struct gka_entry *src, gka_time_t elapsed, int rate,
    int N
);

__PROCESS_BRIDGE__ void
gkaHipSetPhases(gka_decimal_t *dest, double *steps, int period_size, int N);

__PROCESS_BRIDGE__ void gkaHipProcessBlock(
    gka_decimal_t *dest, struct gka_entry *src, double *phases,
    gka_time_t elapsed, int rate, int period_size
);

__PROCESS_HOST__ void gka_process_audio_hip(
    double *dest, struct gka_entry *src, int count, int rate, gka_time_t elapsed
);