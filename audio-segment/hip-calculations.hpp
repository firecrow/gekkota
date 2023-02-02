#include "audio-segment.h"
#include "seg-management.c"
#include "retrieve-seg.c"
#include "math-transitions.c"

/**
 * This function gets the frame value frame value for this sound event, using
 * the pre calculated phase of the sound
 */
__PROCESS_GPU__ gka_decimal_t gka_get_frame_value_from_event_hipdevice(
    struct gka_entry *blk, double *phases, struct gka_entry *event, int soundId,
    gka_time_t start, int frame, gka_time_t local, const uint32_t rate,
    int period_size
);

/**
 * This function retrieves the step (delta between the last phase position) for
 * each frame based on the frequency of the sound
 */
__PROCESS_GPU__ double gka_get_frame_step_from_event_hipdevice(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start, int frame,
    gka_time_t local, const uint32_t rate
);

/**
 * This retrieves an entire frame with all sounds mixed in, using the
 * pre-calculated phase information
 */
__PROCESS_GPU__ gka_decimal_t gka_frame_from_block_hipdevice(
    struct gka_entry *blk, double *phases, int frame, gka_time_t local,
    int rate, int period_size
);

/**
 * This function calculates the steps (position since last phase) for each sound
 * and stores it for use by other calculations
 */
__PROCESS_GPU__ void gka_set_steps_from_hipdevice(
    struct gka_entry *blk, double *out, int frame, gka_time_t local, int rate,
    int period_size
);

/**
 * This function generates the phase positions for all sounds in this block,
 * based on the pre-calculated step changes
 */
__PROCESS_GPU__ void gka_set_phases_for_event_hipdevice(
    double *dest, double *steps, int soundId, int period_size
);

/****************************************************************************
 * KERNELS - these are the kernels that are callable from the host and run the
 * GPU code
 ***************************************************************************/

/**
 * This kernel populates the step data based on the frequency (change since last
 * phase position) for all sounds in a block - this process is parallel to be
 * one GPU thread per frame
 */
__PROCESS_BRIDGE__ void gkaHipSetSteps(
    gka_decimal_t *dest, struct gka_entry *src, gka_time_t elapsed, int rate,
    int N
);

/**
 * This kernel populates the phase positions based on the pre-populated step
 * changes - becuase phase positions are periodic and thereby in-series, this
 * calculation happens on the sound dimension, as one GPU thread per sound
 */
__PROCESS_BRIDGE__ void
gkaHipSetPhases(gka_decimal_t *dest, double *steps, int period_size, int N);

/**
 * This kernel is the final step and calculates the frames with all the sounds
 * of that time offset mixed into one value per frame, this also runs on GPU
 * thread per frame
 */
__PROCESS_BRIDGE__ void gkaHipProcessBlock(
    gka_decimal_t *dest, struct gka_entry *src, double *phases,
    gka_time_t elapsed, int rate, int period_size
);
