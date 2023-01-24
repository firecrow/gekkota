#include "block-sound-mem/block-sound-mem.h"

__device__ void linear_ease_hipdevice(
    double *progress, struct gka_entry *segment, double start, double end
) {}

__device__ void square_ease_hipdevice(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress = start;
}

__device__ void ease_in_hipdevice(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress *= (*progress);
}

__device__ void ease_flip_hipdevice(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress = 1 - *progress;
}

__device__ void ease_out_hipdevice(
    double *progress, struct gka_entry *segment, double start, double end
) {
  double base = 1 - *progress;
  double curved = base * base;
  *progress *= 1 - curved;
}

__device__ void ease_inout_hipdevice(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress *= (*progress);
}

__device__ void gka_apply_transition_hipdevice(
    double *progress, gka_operand_t transition, struct gka_entry *segment,
    double start, double end
) {
  switch (transition) {
  case GKA_CLIFF:
    square_ease_hipdevice(progress, segment, start, end);
    break;
  case GKA_LINEAR:
    linear_ease_hipdevice(progress, segment, start, end);
    break;
  case GKA_EASE_IN:
    ease_in_hipdevice(progress, segment, start, end);
    break;
  case GKA_EASE_OUT:
    ease_out_hipdevice(progress, segment, start, end);
    break;
  }
}

__device__ gka_local_address_t
gka_to_local_hipdevice(struct gka_entry *blk, struct gka_entry *entry) {
  return (gka_global_t)entry - ((gka_global_t)blk);
}

__device__ struct gka_entry *
gka_pointer_hipdevice(struct gka_entry *blk, gka_local_address_t localp) {
  // if (localp < 0 || localp > blk->allocated) {
  //   return GKA_BOUNDRY_ACTION;
  // }
  return (struct gka_entry *)((char *)blk + localp);
}

__device__ gka_local_address_t
gka_next_local_hipdevice(struct gka_entry *blk, gka_local_address_t localp) {
  gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
  // if (next_would_be > blk->allocated) {
  //   return GKA_BOUNDRY_ACTION;
  // }
  return next_would_be;
}

__device__ gka_local_address_t gka_entry_next_hipdevice(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
) {
  if (0) {
    printf("finding next from localp: %ld\n", localp / GKA_SEGMENT_SIZE);
  }

  gka_local_address_t newlp = gka_next_local_hipdevice(blk, localp);
  if (newlp == GKA_BOUNDRY_ACTION) {
    return GKA_BOUNDRY_ACTION;
  }
  struct gka_entry *s = gka_pointer_hipdevice(blk, newlp);
  if (s->values.all.type == GKA_NEXT_LOCAL) {
    return s->values.link.addr;
  } else if (s->values.all.type == type) {
    if (0) {
      printf("found newlp: %ld\n", newlp / GKA_SEGMENT_SIZE);
    }
    return newlp;
  }
  return GKA_BOUNDRY_ACTION;
}

__device__ struct gka_entry *gka_segment_from_pattern_hipdevice(
    struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t offset
) {
  struct gka_entry *next = gka_pointer_hipdevice(blk, currentlp);
  struct gka_entry *current = next;
  gka_local_address_t nextlp = 0;
  while (next && next->values.segment.start_time < offset) {
    current = next;
    nextlp = gka_entry_next_hipdevice(
        blk, gka_to_local_hipdevice(blk, next), GKA_SEGMENT_VALUE
    );
    if (nextlp == GKA_BOUNDRY_ACTION) {
      next = NULL;
    } else {
      next = gka_pointer_hipdevice(blk, nextlp);
    }
  }

  return current;
}

__device__ double value_from_segment_hipdevice(
    struct gka_entry *blk, gka_local_address_t current, double base_value,
    gka_time_t offset
) {

  struct gka_entry *segment =
      gka_segment_from_pattern_hipdevice(blk, current, offset);
  struct gka_entry *next = NULL;

  if (segment->values.segment.start_time < offset) {
    gka_local_address_t nextlp = gka_entry_next_hipdevice(
        blk, gka_to_local_hipdevice(blk, segment), GKA_SEGMENT_VALUE
    );
    if (nextlp) {
      next = gka_pointer_hipdevice(blk, nextlp);
    }
  }

  if (!next) {
    return base_value * segment->values.segment.value;
  }

  double progress =
      (double)(offset - segment->values.segment.start_time) /
      (double
      )(next->values.segment.start_time - segment->values.segment.start_time);

  gka_apply_transition_hipdevice(
      &progress, segment->values.segment.transition, segment,
      segment->values.segment.value, next->values.segment.value
  );

  double segment_value =
      segment->values.segment.value +
      ((next->values.segment.value - segment->values.segment.value) * progress);

  return base_value * segment_value;
}

__device__ gka_decimal_t gka_get_frame_value_from_event_hipdevice(
    struct gka_entry *blk, double *phases, struct gka_entry *event, int soundId,
    gka_time_t start, int frame, gka_time_t local, const uint32_t rate,
    int period_size
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  int idx = 0;

  double position = local - start;
  struct gka_entry *s = gka_pointer_hipdevice(blk, soundlp);

  double freq =
      value_from_segment_hipdevice(blk, s->values.sound.freq, base, position);

  double volume =
      value_from_segment_hipdevice(blk, s->values.sound.volume, base, position);

  if (volume > 1.0 || volume < 0.0) {
    printf("ERROR WITH VOLUME:%lf\n", volume);
    volume = 1.0;
  }

  // used to retrieve pre calculated phases
  int slot = soundId * period_size + frame;
  return (double)sin(phases[slot]) * volume;
}

__device__ double gka_get_frame_step_from_event_hipdevice(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start, int frame,
    gka_time_t local, const uint32_t rate
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  int idx = 0;

  double position = local - start;
  struct gka_entry *s = gka_pointer_hipdevice(blk, soundlp);

  double freq =
      value_from_segment_hipdevice(blk, s->values.sound.freq, base, position);

  return MAX_PHASE * freq / (double)rate;
}

__device__ gka_time_t
gka_time_modulus_hipdevice(gka_time_t src, gka_time_t mod) {
  gka_time_t remainder = src;
  if (src > mod) {
    gka_time_t times = src / mod;
    remainder = src - mod * times;
  }
  return remainder;
}

__device__ gka_decimal_t gka_frame_from_block_hipdevice(
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

    soundlp = gka_entry_next_hipdevice(blk, soundlp, GKA_SOUND_EVENT);
    soundId++;
  }
  return frame_value;
}

__device__ void gka_set_steps_from_block_hipdevice(
    struct gka_entry *blk, double *out, int frame, gka_time_t local, int rate,
    int period_size
) {

  gka_decimal_t frame_value = 0;
  struct gka_entry *head = gka_pointer_hipdevice(blk, 0);
  gka_local_address_t soundlp = head->values.head.addr;
  int soundId = 0;
  int slot = 0;

  while (soundlp) {
    slot = soundId * period_size + frame;

    struct gka_entry *e = gka_pointer_hipdevice(blk, soundlp);
    if (e->values.event.start > local) {
      continue;
    }
    if (e->values.event.repeat) {
      gka_time_t local_repeat;
      local_repeat = gka_time_modulus_hipdevice(
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

    soundlp = gka_entry_next_hipdevice(blk, soundlp, GKA_SOUND_EVENT);
    soundId++;
  }
}

__device__ void gka_set_phases_for_event_hipdevice(
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