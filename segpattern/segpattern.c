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

gka_local_address_t gka_segpattern_get_next_segment(struct gka_mem_block *blk, gka_local_address_t localp){
  printf("getting next from %d........\n", localp);
  struct gka_entry *next = gka_pointer(blk, localp+sizeof(struct gka_entry));
  if(next->type == GKA_SEGMENT_VALUE){
    return gka_to_local(blk, next);
  }else if(next->type == GKA_NEXT_LOCAL){
    return next->addr;
  }
  return GKA_BOUNDRY_ACTION;
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

void gka_set_entry_status(struct gka_mem_block *blk, gka_local_address_t localp, gka_operand_t type) {
  struct gka_entry *s = gka_pointer(blk, localp);
  s->type = type;
}

gka_local_address_t gka_pattern_create(struct gka_mem_block *blk) {
  gka_local_address_t localp =
      gka_allocate_space(blk, sizeof(struct gka_entry));

  if (localp == GKA_BOUNDRY_ACTION) {
    fprintf(stderr, "Error allocating segment %s:%d\n", __FILE__, __LINE__);
    return GKA_MEMORY_FAILURE;
  }

  printf("create pattern\n");

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->type = GKA_SOUND;
  //gka_set_entry_status(blk, localp+sizeof(struct gka_entry), GKA_RESERVED_BY_NEIGHBOUR);

  return localp;
}

gka_local_address_t gka_extend_segment(
     struct gka_mem_block *blk, gka_local_address_t current, struct gka_entry *seg){
  printf("\x1b[31mextending for segment with value of %lf\x1b[0m\n", seg->values.placement.value);
  gka_local_address_t neighbour_address = gka_next_local(blk, current);
  printf("\x1b[31mallocated:%ld neighbour address:%ld %lf\x1b[0m\n", blk->allocated, neighbour_address, seg->values.placement.value);

  if(neighbour_address == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "BLOCK BOUNDS REACHED %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *neighbour = gka_pointer(blk, neighbour_address);

  gka_local_address_t next_neighbour_address = gka_next_local(blk, neighbour_address);
  if(next_neighbour_address == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "BLOCK BOUNDS REACHED %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *next_neighbour = gka_pointer(blk, next_neighbour_address);

  if(neighbour->type != GKA_UNSPECIFIED && neighbour->type != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER %s:%d\n", __FILE__, __LINE__);
    exit(1);
    return 0;
  }
  printf("n(%ld):%ld nn(%ld):%ld\n", neighbour_address, neighbour->type, next_neighbour_address, next_neighbour->type);
  if(next_neighbour_address == GKA_BOUNDRY_ACTION || next_neighbour->type == GKA_UNSPECIFIED){
    if(!gka_claim_entry(blk, next_neighbour_address)){
      fprintf(stderr, "FATAL MEMORY CLOBBER %s:%d\n", __FILE__, __LINE__);
      exit(1);
      return 0;
    }

    printf("\x1b[36musing neighbour %lf\x1b[0m\n", seg->values.placement.value);

    neighbour->type = GKA_SEGMENT_VALUE;
    neighbour->values.placement.start_time = seg->values.placement.start_time;
    neighbour->values.placement.value = seg->values.placement.value;
    neighbour->transition = seg->transition;

    gka_set_entry_status(blk, next_neighbour_address, GKA_RESERVED_BY_NEIGHBOUR);
    next_neighbour->type = GKA_RESERVED_BY_NEIGHBOUR;
    return neighbour_address;
  }else{
    printf("\x1b[36madding link neighbour %lf\x1b[0m\n", seg->values.placement.value);
    if(neighbour->type == GKA_RESERVED_BY_NEIGHBOUR){
      gka_local_address_t newlp = gka_segment_create(
        blk, seg->values.placement.start_time, seg->values.placement.value, seg->transition);

      neighbour->type = GKA_NEXT_LOCAL;
      neighbour->addr = newlp;

      struct gka_entry *s = (struct gka_entry*)gka_pointer(blk, newlp);

      s->values.placement.start_time = seg->values.placement.start_time;
      s->values.placement.value = seg->values.placement.value;
      s->transition = seg->transition;
      return newlp;
    }
  }
  return 0;
}

gka_local_address_t gka_entry_next(struct gka_mem_block *blk, gka_local_address_t localp, gka_operand_t type){
  printf("\x1b[34mgetting%ld\x1b[0m\n", localp);
  gka_local_address_t newlp = gka_next_local(blk, localp);
  if(newlp == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "Error getting next address %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *s = gka_pointer(blk, newlp);
  if(s->type == GKA_NEXT_LOCAL){
    printf("returning link addresss\n");
    return s->addr;
  }else if(s->type == type){
    printf("returning local addresss\n");
    return newlp;
  }
  printf("returning no addresss\n");
  return GKA_BOUNDRY_ACTION;
}

gka_local_address_t gka_segpattern_add_segment(struct gka_mem_block *blk, struct gka_entry *pattern, struct gka_entry *seg) {
  if (pattern->addr == NULL) {
    pattern->addr = gka_segment_create(
      blk, seg->values.placement.start_time, seg->values.placement.value, seg->transition);
    return pattern->addr;
  } else {
    gka_local_address_t next = pattern->addr;
    gka_local_address_t current = next;
    while(next){
      current = next;
      next = gka_entry_next(blk, next, GKA_SEGMENT_VALUE);
    }
    printf("pursuing a record just after: %ld\n", current);
    return gka_extend_segment(blk, current, seg);
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
