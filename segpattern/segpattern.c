#include <stdio.h>
#include "../block-sound-mem/block-sound-mem.h"
#include "segpattern.h"

void linear_ease(
    double *progress, struct gka_entry *segment, double start, double end
) {}

void square_ease(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress = start;
}

void ease_in(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress *= (*progress);
}

void ease_flip(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress = 1 - *progress;
}

void ease_out(
    double *progress, struct gka_entry *segment, double start, double end
) {
  double base = 1 - *progress;
  double curved = base * base;
  *progress *= 1 - curved;
}

void ease_inout(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress *= (*progress);
}

gka_apply_transition(
    double *progress, gka_operand_t transition, struct gka_entry *segment, double start, double end
){
  switch(transition){
    case GKA_CLIFF:
      square_ease(progress, segment, start, end); 
      break;
    case GKA_LINEAR:
      linear_ease(progress, segment, start, end); 
      break;
    case GKA_EASE_IN:
      ease_in(progress, segment, start, end); 
      break;
    case GKA_EASE_OUT:
      ease_out(progress, segment, start, end); 
      break;
  }
}


gka_local_address_t gka_segment_create(
        struct gka_mem_block *blk, gka_value_t start_time, gka_decimal_t start_value,
        gka_operand_t ease
    ) {
  gka_local_address_t localp =
      gka_allocate_space(blk, sizeof(struct gka_entry));

  if (localp == GKA_BOUNDRY_ACTION) {
    fprintf(stderr, "Error allocating segment %s:%d\n", __FILE__, __LINE__);
    return GKA_MEMORY_FAILURE;
  }
  printf("create\n");

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.placement.start_time = start_time;
  s->values.placement.value = start_value;
  s->transition = ease;
  s->type = GKA_SEGMENT_VALUE;

  return localp;
}

gka_local_address_t gka_segpattern_get_next_segment(struct gka_mem_block *blk, gka_local_address_t local_seg){
  struct gka_entry *next = gka_pointer(blk, local_seg+sizeof(struct gka_entry));
  if(next->type == GKA_SEGMENT_VALUE){
    return next;
  }else if(next->type == GKA_NEXT_LOCAL){
    return next->addr;
  }
  return GKA_BOUNDRY_ACTION;
}

int gka_create_link(struct gka_mem_block *blk, gka_local_address_t place){
  struct gka_entry *entry = gka_pointer(blk, place);
  if(entry->type != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER");
    exit(1);
    return 0;
  }
  gka_local_address_t newlp = gka_allocate_space(blk, sizeof(struct gka_entry));
  entry->type = GKA_NEXT_LOCAL;
  entry->addr = newlp;

  return newlp;
}

int gka_extend_pattern(
     struct gka_mem_block *blk, struct gka_entry *pattern, gka_local_address_t current, struct gka_entry *seg){
  struct gka_entry *neighbour = gka_pointer(blk, current+sizeof(struct gka_entry));
  struct gka_entry *next_neighbour = gka_pointer(blk, current+sizeof(struct gka_entry)*2);
  if(neighbour->type != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER");
    exit(1);
    return 0;
  }
  if(next_neighbour->type == GKA_UNSPECIFIED){
    gka_local_address_t newlp = gka_segment_create(
      blk, seg->values.placement.start_time, seg->values.placement.value, seg->transition);
    next_neighbour->type = GKA_RESERVED_BY_NEIGHBOUR;
  }else{
    gka_local_address_t newlp = gka_create_link(blk, current);
    next_neighbour = gka_pointer(blk, newlp+sizeof(struct gka_entry));
    next_neighbour->type = GKA_RESERVED_BY_NEIGHBOUR;

    struct gka_entry *s = (struct gka_entry*)gka_pointer(blk, newlp);

    s->values.placement.start_time = seg->values.placement.start_time;
    s->values.placement.value = seg->values.placement.value;
    s->transition = seg->transition;
  }
}

void gka_segpattern_add_segment(struct gka_mem_block *blk, struct gka_entry *pattern, struct gka_entry *seg) {
  if (pattern->addr == NULL) {
    pattern->addr = gka_segment_create(
      blk, seg->values.placement.start_time, seg->values.placement.value, seg->transition);
  } else {
    struct gka_entry *next = gka_pointer(blk, pattern->addr);
    struct gka_entry *current = next;
    while (next) {
      current = next;
      next = gka_segpattern_get_next_segment(blk, (gka_local_address_t)((uint64_t)next - (uint64_t)blk));
    }
    gka_extend_pattern(pattern, current, (gka_local_address_t)((uint64_t)seg - (uint64_t)next), (gka_local_address_t)((uint64_t)seg - (uint64_t)blk));
  }
}

struct gka_entry *
segment_from_segment(struct gka_mem_block *blk, struct gka_entry *pattern, gka_time_t offset) {
    struct gka_entry *next = gka_pointer(blk, pattern->addr);
    struct gka_entry *current = next;
    while (next) {
      current = next;
      next = gka_segpattern_get_next_segment(blk, (gka_local_address_t)((uint64_t)blk - (uint64_t)next));
    }

  return next;
}

double value_from_segment(
    struct gka_mem_block *blk, struct gka_segpattern *pattern, double base_value, gka_time_t offset
) {

  struct gka_entry *segment = segment_from_segment(blk, pattern, offset);
  struct gka_entry *next = gka_segpattern_get_next_segment(blk, (gka_local_address_t)((uint64_t)blk - (uint64_t)segment));

  if (!next) {
    return base_value * segment->values.placement.value;
  }

  double progress = (double)(offset - segment->values.placement.value) /
                    (double)(next->values.placement.start_time - segment->values.placement.start_time);

  gka_apply_transition(
      &progress, segment->transition, segment, segment->values.placement.value, next->values.placement.value
  );


  double segment_value =
      segment->values.placement.value + (next->values.placement.value - segment->values.placement.value) * progress;

  return base_value * segment_value;
}
