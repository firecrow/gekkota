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
  *progress *= _hipdevice(*progress);
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
  *progress *= _hipdevice(*progress);
}

__device__ gka_apply_transition_hipdevice(
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

__device__ gka_local_address_t gka_entry_next_hipdevice(
    struct gka_mem_block *blk, gka_local_address_t localp, gka_operand_t type
) {
  if (0) {
    printf("finding next from localp: %ld\n", localp / GKA_SEGMENT_SIZE);
  }

  gka_local_address_t newlp = gka_next_local(blk, localp);
  if (newlp == GKA_BOUNDRY_ACTION) {
    fprintf(stderr, "Error getting next address %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *s = gka_pointer(blk, newlp);
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

__device__ double value_from_segment_hipdevice(
    struct gka_mem_block *blk, gka_local_address_t current, double base_value,
    gka_time_t offset
) {

  struct gka_entry *segment =
      gka_segment_from_pattern_hipdevice(blk, current, offset);
  struct gka_entry *next = NULL;

  if_hipdevice(segment->values.segment.start_time < offset) {
    gka_local_address_t nextlp = gka_entry_next_hipdevice(
        blk, gka_to_local(blk, segment), GKA_SEGMENT_VALUE
    );
    if_hipdevice(nextlp) { next = gka_pointer_hipdevice(blk, nextlp); }
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
    struct gka_mem_block *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const uint32_t rate
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  double value = 0.0;
  int idx = 0;
  while (soundlp) {
    double position = local - start;
    struct gka_entry *s = gka_pointer_hipdevice(blk, soundlp);

    double freq =
        value_from_segment_hipdevice(blk, s->values.sound.freq, base, position);
    double volume = value_from_segment_hipdevice(
        blk, s->values.sound.volume, base, position
    );

    if (volume > 1.0 || volume < 0.0) {
      printf_hipdevice("ERROR WITH VOLUME:%lf\n", volume);
      volume = 1.0;
    }

    // reconcile phase for next time
    double step = MAX_PHASE * freq / (double)rate;

    value = _hipdevice(sin(s->values.sound.phase) * volume);

    s->values.sound.phase += step;

    if (s->values.sound.phase >= MAX_PHASE) {
      s->values.sound.phase -= MAX_PHASE;
    }

    // increment to next sound
    soundlp = gka_entry_next_hipdevice(blk, soundlp, GKA_SOUND);
  }
  return value;
}

__device__ gka_decimal_t gka_frame_from_block_hipdevice(
    struct gka_mem_block *blk, gka_time_t local, int rate
) {
  gka_decimal_t frame_value = 0;
  struct gka_entry *head = gka_pointer_hipdevice(blk, 0);
  gka_local_address_t soundlp = head->values.link.addr;
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
          blk, e, 0, local_repeat, rate
      );
    } else {
      frame_value += gka_get_frame_value_from_event_hipdevice(
          blk, e, e->values.event.start, local, rate
      );
    }

    soundlp = gka_entry_next_hipdevice(blk, soundlp, GKA_SOUND_EVENT);
  }
  return frame_value;
}