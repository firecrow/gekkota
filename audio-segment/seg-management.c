#include "audio-segment.h"

__PROCESS_BOTH__ struct gka_entry *gka_alloc_memblock(gka_local_address_t size
) {
  struct gka_entry *m = (struct gka_entry *)malloc(size);
  if (m == NULL) {
    return GKA_BOUNDRY_ACTION;
  }
  memset(m, 0, size);

  m->values.head.next_available = GKA_SEGMENT_SIZE;
  m->values.head.allocated = size;
  m->values.all.type = GKA_HEAD;

  return m;
}

__PROCESS_BOTH__ gka_local_address_t
gka_allocate_space(struct gka_entry *blk, gka_local_address_t size) {
  if (blk->values.head.next_available + size > blk->values.head.allocated) {
    return GKA_BOUNDRY_ACTION;
  }
  gka_local_address_t next = blk->values.head.next_available;
  blk->values.head.next_available += size;
  return next;
}

__PROCESS_BOTH__ gka_local_address_t
gka_to_local(struct gka_entry *blk, struct gka_entry *entry) {
  return (gka_global_t)entry - ((gka_global_t)blk);
}

__PROCESS_BOTH__ struct gka_entry *
gka_pointer(struct gka_entry *blk, gka_local_address_t localp) {
  if (localp < 0 || localp > blk->values.head.allocated) {
    return GKA_BOUNDRY_ACTION;
  }
  return (struct gka_entry *)((gka_global_t)blk + localp);
}

__PROCESS_BOTH__ struct gka_entry *gka_nth(struct gka_entry *blk, int offset) {
  return gka_pointer(blk, GKA_SEGMENT_SIZE * offset);
}

__PROCESS_BOTH__ int
gka_claim_entry(struct gka_entry *blk, gka_local_address_t localp) {
  gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
  if (next_would_be > blk->values.head.allocated) {
    return GKA_BOUNDRY_ACTION;
  }
  if (blk->values.head.next_available < next_would_be) {
    blk->values.head.next_available = next_would_be;
  }
  return next_would_be;
}

__PROCESS_BOTH__ struct gka_entry *
gka_next(struct gka_entry *blk, gka_local_address_t localp) {
  gka_local_address_t next_would_be = gka_next_local(blk, localp);
  if (next_would_be == GKA_BOUNDRY_ACTION) {
    return GKA_BOUNDRY_ACTION;
  }
  return gka_pointer(blk, next_would_be);
}

__PROCESS_BOTH__ gka_local_address_t
gka_next_local(struct gka_entry *blk, gka_local_address_t localp) {
  gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
  if (next_would_be > blk->values.head.allocated) {
    return GKA_BOUNDRY_ACTION;
  }
  return next_would_be;
}

__PROCESS_BOTH__ gka_local_address_t
gka_extend_entry(struct gka_entry *blk, gka_local_address_t localp) {

  gka_local_address_t neighbour_address = gka_next_local(blk, localp);

  if (neighbour_address == GKA_BOUNDRY_ACTION) {
    return GKA_BOUNDRY_ACTION;
  }
  struct gka_entry *neighbour = gka_pointer(blk, neighbour_address);

  gka_local_address_t next_neighbour_address =
      gka_next_local(blk, neighbour_address);
  if (next_neighbour_address == GKA_BOUNDRY_ACTION) {
    return GKA_BOUNDRY_ACTION;
  }
  struct gka_entry *next_neighbour = gka_pointer(blk, next_neighbour_address);

  if (neighbour->values.all.type != GKA_UNSPECIFIED &&
      neighbour->values.all.type != GKA_RESERVED_BY_NEIGHBOUR) {
    return GKA_BOUNDRY_ACTION;
  }

  if (next_neighbour_address == GKA_BOUNDRY_ACTION ||
      next_neighbour->values.all.type == GKA_UNSPECIFIED) {
    if (!gka_claim_entry(blk, next_neighbour_address)) {
      return GKA_BOUNDRY_ACTION;
    }

    gka_set_entry_status(
        blk, next_neighbour_address, GKA_RESERVED_BY_NEIGHBOUR
    );
    next_neighbour->values.all.type = GKA_RESERVED_BY_NEIGHBOUR;

    return neighbour_address;
  } else if (neighbour->values.all.type == GKA_RESERVED_BY_NEIGHBOUR) {
    gka_local_address_t newlp = gka_allocate_space(blk, GKA_SEGMENT_SIZE);
    neighbour->values.all.type = GKA_NEXT_LOCAL;
    neighbour->values.link.addr = newlp;

    return newlp;
  }
  return GKA_BOUNDRY_ACTION;
}

__PROCESS_BOTH__ gka_local_address_t gka_add_entry_to_set(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
) {
  gka_local_address_t next = localp;
  gka_local_address_t last = next;
  while (next) {
    last = next;
    next = gka_entry_next(blk, localp, type);
  }
  return gka_extend_entry(blk, last);
}

__PROCESS_BOTH__ void gka_set_entry_status(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
) {
  struct gka_entry *s = gka_pointer(blk, localp);
  s->values.all.type = type;
}

__PROCESS_BOTH__ gka_local_address_t gka_entry_next(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
) {
  if (0) {
    printf("finding next from localp: %ld\n", localp / GKA_SEGMENT_SIZE);
  }

  gka_local_address_t newlp = gka_next_local(blk, localp);
  if (newlp == GKA_BOUNDRY_ACTION) {
    return GKA_BOUNDRY_ACTION;
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

__PROCESS_BOTH__ gka_local_address_t gka_segment_create(
    struct gka_entry *blk, gka_value_t start_time, gka_decimal_t start_value,
    gka_operand_t ease
) {
  gka_local_address_t localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE);

  if (localp == GKA_BOUNDRY_ACTION) {
    return GKA_MEMORY_FAILURE;
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.segment.start_time = start_time;
  s->values.segment.value = start_value;
  s->values.segment.transition = ease;
  s->values.all.type = GKA_SEGMENT_VALUE;

  return localp;
}

__PROCESS_BOTH__ gka_local_address_t gka_segment_new(
    struct gka_entry *blk, gka_value_t start_time, gka_decimal_t start_value,
    gka_operand_t ease
) {
  gka_local_address_t localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE * 2);

  if (localp == GKA_BOUNDRY_ACTION) {
    return GKA_MEMORY_FAILURE;
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.segment.start_time = start_time;
  s->values.segment.value = start_value;
  s->values.segment.transition = ease;
  s->values.all.type = GKA_SEGMENT_VALUE;

  gka_local_address_t next_address = gka_next_local(blk, localp);
  gka_set_entry_status(blk, next_address, GKA_RESERVED_BY_NEIGHBOUR);

  return localp;
}

__PROCESS_BOTH__ gka_local_address_t gka_segpattern_add_segment(
    struct gka_entry *blk, gka_local_address_t currentlp, struct gka_entry *seg
) {
  gka_local_address_t next = currentlp;
  gka_local_address_t current = next;
  while (next) {
    current = next;
    next = gka_entry_next(blk, next, GKA_SEGMENT_VALUE);
  }
  return gka_extend_segment(blk, current, seg);
}

__PROCESS_BOTH__ gka_local_address_t gka_segpattern_add_segment_values(
    struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t start_time,
    gka_decimal_t value, gka_operand_t transition
) {

  struct gka_entry _s;
  _s.values.all.type = GKA_SEGMENT_VALUE;
  _s.values.segment.start_time = start_time;
  _s.values.segment.value = value;

  return gka_segpattern_add_segment(blk, currentlp, &_s);
}

__PROCESS_BOTH__ gka_local_address_t gka_extend_segment(
    struct gka_entry *blk, gka_local_address_t current, struct gka_entry *seg
) {
  gka_local_address_t newlp = gka_extend_entry(blk, current);
  if (newlp == GKA_BOUNDRY_ACTION) {
    return GKA_BOUNDRY_ACTION;
  }

  struct gka_entry *s = gka_pointer(blk, newlp);
  s->values.all.type = GKA_SEGMENT_VALUE;
  s->values.segment.start_time = seg->values.segment.start_time;
  s->values.segment.value = seg->values.segment.value;
  s->values.segment.transition = seg->values.segment.transition;

  return newlp;
}

__PROCESS_BOTH__ gka_local_address_t gka_sound_create(
    struct gka_entry *blk, gka_local_address_t freq, gka_local_address_t volume
) {

  gka_local_address_t localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE);

  if (localp == GKA_BOUNDRY_ACTION) {
    return GKA_MEMORY_FAILURE;
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.sound.freq = freq;
  s->values.sound.volume = volume;
  s->values.all.type = GKA_SOUND;

  return localp;
};

__PROCESS_BOTH__ gka_local_address_t gka_sound_event_create(
    struct gka_entry *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
) {

  gka_local_address_t localp = 0;

  struct gka_entry *head = gka_pointer(blk, 0);
  if (head->values.all.type == GKA_HEAD) {

    localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE * 2);
    if (localp == GKA_BOUNDRY_ACTION) {
      return GKA_MEMORY_FAILURE;
    }

    printf("?\n");
    head->values.head.addr = localp;

    gka_local_address_t neighbour_address = gka_next_local(blk, localp);
    gka_set_entry_status(blk, neighbour_address, GKA_RESERVED_BY_NEIGHBOUR);

  } else if (head->values.all.type == GKA_NEXT_LOCAL) {
    localp = gka_add_entry_to_set(blk, head->values.link.addr, GKA_SOUND_EVENT);
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.event.start = start;
  s->values.event.repeat = repeat;
  s->values.event.sounds = sounds;
  s->values.all.type = GKA_SOUND_EVENT;

  return localp;
}
