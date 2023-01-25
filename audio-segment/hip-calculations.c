#include "hip-calculations.h"

__PROCESS_GPU__ gka_decimal_t gka_get_frame_value_from_event_hipdevice(
    struct gka_entry *blk, double *phases, struct gka_entry *event, int soundId,
    gka_time_t start, int frame, gka_time_t local, const uint32_t rate,
    int period_size
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  int idx = 0;

  double position = local - start;
  struct gka_entry *s = gka_pointer(blk, soundlp);

  double volume =
      value_from_segment(blk, s->values.sound.volume, base, position);

  if (volume > 1.0 || volume < 0.0) {
    printf("ERROR WITH VOLUME:%lf\n", volume);
    volume = 1.0;
  }

  // used to retrieve pre calculated phases
  int slot = soundId * period_size + frame;
  return (double)sin(phases[slot]) * 0.2; // volume;
}

__PROCESS_GPU__ double gka_get_frame_step_from_event_hipdevice(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start, int frame,
    gka_time_t local, const uint32_t rate
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  int idx = 0;

  double position = local - start;
  struct gka_entry *s = gka_pointer(blk, soundlp);

  double freq =
      value_from_segment(blk, s->values.sound.freq, base, position);

  return MAX_PHASE * freq / (double)rate;
}

__PROCESS_GPU__ gka_decimal_t gka_frame_from_block_hipdevice(
    struct gka_entry *blk, double *phases, int frame, gka_time_t local,
    int rate, int period_size
) {

  gka_decimal_t frame_value = 0;
  struct gka_entry *head = gka_pointer_hipdevice(blk, 0);
  gka_local_address_t soundlp = head->values.head.addr;

  int soundId = 0;

  while (soundlp) {
    struct gka_entry *e = gka_pointer_hipdevice(blk, soundlp);
    if (e->values.event.start > local) {
      continue;
    }
    if (e->values.event.repeat) {
      gka_time_t local_repeat;
      local_repeat = gka_time_modulus_hipdevice(
          local - e->values.event.start, e->values.event.repeat
      );

      frame_value += gka_get_frame_value_from_event_hipdevice(
          blk, phases, e, soundId, 0, frame, local_repeat, rate, period_size
      );
    } else {
      frame_value += gka_get_frame_value_from_event_hipdevice(
          blk, phases, e, soundId, e->values.event.start, frame, local, rate,
          period_size
      );
    }

    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND_EVENT);
    soundId++;
  }
  return frame_value;
}

__PROCESS_GPU__ void gka_set_steps_from_hipdevice(
    struct gka_entry *blk, double *out, int frame, gka_time_t local, int rate,
    int period_size
) {

  gka_decimal_t frame_value = 0;
  struct gka_entry *head = gka_pointer(blk, 0);
  gka_local_address_t soundlp = head->values.head.addr;
  int soundId = 0;
  int slot = 0;

  while (soundlp) {
    slot = soundId * period_size + frame;

    struct gka_entry *e = gka_pointer(blk, soundlp);
    if (e->values.event.start > local) {
      continue;
    }
    if (e->values.event.repeat) {
      gka_time_t local_repeat;
      local_repeat = gka_time_modulus(
          local - e->values.event.start, e->values.event.repeat
      );

      out[slot] = gka_get_frame_step_from_event_hipdevice(
          blk, e, 0, frame, local_repeat, rate
      );

    } else {
      out[slot] = gka_get_frame_step_from_event_hipdevice(
          blk, e, e->values.event.start, frame, local, rate
      );
    }

    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND_EVENT);
    soundId++;
  }
}

__PROCESS_GPU__ void gka_set_phases_for_event_hipdevice(
    double *dest, double *steps, int soundId, int period_size
) {
  int slot;
  double phase = 0.0;

  for (int frame = 0; frame < period_size; frame++) {
    slot = soundId * period_size + frame;

    phase += steps[slot];

    if (phase >= MAX_PHASE) {
      phase -= MAX_PHASE;
    }

    dest[slot] = phase;
  }
}

/* ------ kernels ------ */

__PROCESS_BRIDGE__ void gkaHipSetSteps(
    gka_decimal_t *dest, struct gka_entry *src, gka_time_t elapsed, int rate,
    int N
) {
  int frameId = hipThreadIdx_x + hipBlockDim_x * hipBlockIdx_x;
  if (frameId >= N)
    return;

  gka_time_t local = elapsed + frameId;
  gka_set_steps_from_block_hipdevice(src, dest, frameId, local, rate, N);
}

__PROCESS_BRIDGE__ void
gkaHipSetPhases(gka_decimal_t *dest, double *steps, int period_size, int N) {
  int frameId = hipThreadIdx_x + hipBlockDim_x * hipBlockIdx_x;
  if (frameId >= N)
    return;

  gka_set_phases_for_event_hipdevice(dest, steps, frameId, period_size);
}

__PROCESS_BRIDGE__ void gkaHipProcessBlock(
    gka_decimal_t *dest, struct gka_entry *src, double *phases,
    gka_time_t elapsed, int rate, int period_size
) {
  int frameId = hipThreadIdx_x + hipBlockDim_x * hipBlockIdx_x;
  gka_time_t local = elapsed + frameId;
  dest[frameId] = gka_frame_from_block_hipdevice(
      src, phases, frameId, local, rate, period_size
  );
}