#include "block-sound-mem.h"
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
      gka_allocate_space(blk, GKA_SEGMENT_SIZE);

  if (localp == GKA_BOUNDRY_ACTION) {
    fprintf(stderr, "Error allocating segment %s:%d\n", __FILE__, __LINE__);
    return GKA_MEMORY_FAILURE;
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.segment.start_time = start_time;
  s->values.segment.value = start_value;
  s->values.segment.transition = ease;
  s->values.all.type = GKA_SEGMENT_VALUE;

  return localp;
}

void gka_set_entry_status(struct gka_mem_block *blk, gka_local_address_t localp, gka_operand_t type) {
  struct gka_entry *s = gka_pointer(blk, localp);
  s->values.all.type = type;
}

gka_local_address_t gka_extend_segment(
     struct gka_mem_block *blk, gka_local_address_t current, struct gka_entry *seg){
  gka_local_address_t neighbour_address = gka_next_local(blk, current);

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

  if(neighbour->values.all.type != GKA_UNSPECIFIED && neighbour->values.all.type != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER %s:%d\n", __FILE__, __LINE__);
    exit(1);
    return 0;
  }
  if(next_neighbour_address == GKA_BOUNDRY_ACTION || next_neighbour->values.all.type == GKA_UNSPECIFIED){
    if(!gka_claim_entry(blk, next_neighbour_address)){
      fprintf(stderr, "FATAL MEMORY CLOBBER %s:%d\n", __FILE__, __LINE__);
      exit(1);
      return 0;
    }


    neighbour->values.all.type = GKA_SEGMENT_VALUE;
    neighbour->values.segment.start_time = seg->values.segment.start_time;
    neighbour->values.segment.value = seg->values.segment.value;
    neighbour->values.segment.transition = seg->values.segment.transition;

    gka_set_entry_status(blk, next_neighbour_address, GKA_RESERVED_BY_NEIGHBOUR);
    next_neighbour->values.all.type = GKA_RESERVED_BY_NEIGHBOUR;
    return neighbour_address;
  }else{
    if(neighbour->values.all.type == GKA_RESERVED_BY_NEIGHBOUR){
      gka_local_address_t newlp = gka_segment_create(
        blk, seg->values.segment.start_time, seg->values.segment.value, seg->values.segment.transition);

      neighbour->values.all.type = GKA_NEXT_LOCAL;
      neighbour->values.link.addr = newlp;

      struct gka_entry *s = (struct gka_entry*)gka_pointer(blk, newlp);

      s->values.segment.start_time = seg->values.segment.start_time;
      s->values.segment.value = seg->values.segment.value;
      s->values.segment.transition = seg->values.segment.transition;
      return newlp;
    }
  }
  return 0;
}

gka_local_address_t gka_entry_next(struct gka_mem_block *blk, gka_local_address_t localp, gka_operand_t type){
  if(0){
    printf("finding next from localp: %ld\n", localp/GKA_SEGMENT_SIZE);
  }

  gka_local_address_t newlp = gka_next_local(blk, localp);
  if(newlp == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "Error getting next address %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *s = gka_pointer(blk, newlp);
  if(s->values.all.type == GKA_NEXT_LOCAL){
    return s->values.link.addr;
  }else if(s->values.all.type == type){
    if(0){
      printf("found newlp: %ld\n", newlp/ GKA_SEGMENT_SIZE);
    }
    return newlp;
  }
  return GKA_BOUNDRY_ACTION;
}

gka_local_address_t gka_segpattern_add_segment(struct gka_mem_block *blk, gka_local_address_t currentlp, struct gka_entry *seg) {
  gka_local_address_t next = currentlp;
  gka_local_address_t current = next;
  while(next){
    current = next;
    next = gka_entry_next(blk, next, GKA_SEGMENT_VALUE);
  }
  return gka_extend_segment(blk, current, seg);
}

gka_local_address_t gka_segpattern_add_segment_values(
    struct gka_mem_block *blk, gka_local_address_t currentlp,
    gka_time_t start_time, gka_decimal_t value, gka_operand_t transition
  ) {

  struct gka_entry _s;
  _s.values.all.type = GKA_SEGMENT_VALUE;
  _s.values.segment.start_time = start_time;
  _s.values.segment.value = value;

  return gka_segpattern_add_segment(blk, currentlp, &_s);
}

struct gka_entry *
gka_segment_from_pattern(struct gka_mem_block *blk, gka_local_address_t currentlp, gka_time_t offset) {
  struct gka_entry *next = gka_pointer(blk, currentlp);
  struct gka_entry *current = next;
  gka_local_address_t nextlp = 0;
  while (next && next->values.segment.start_time < offset) {
    current = next;
    nextlp = gka_entry_next(blk, gka_to_local(blk, next), GKA_SEGMENT_VALUE);
    if(nextlp == GKA_BOUNDRY_ACTION){
      next = NULL;
    }else{
      next = gka_pointer(blk, nextlp);
    }
  }

  return current;
}

double value_from_segment(
    struct gka_mem_block *blk, gka_local_address_t current, double base_value, gka_time_t offset
) {
  if(0){
    printf("value from segment called %ld\n", offset);
  }

  struct gka_entry *segment = gka_segment_from_pattern(blk, current, offset);
  struct gka_entry *next = NULL;

  if(segment->values.segment.start_time < offset){
    gka_local_address_t nextlp = gka_entry_next(blk, gka_to_local(blk, segment), GKA_SEGMENT_VALUE);
    next = gka_pointer(blk, nextlp);
  }

  if(0){
    test_print_entry(segment);
    printf("\n");
    test_print_entry(next);
    printf("\n");
  }

  if (!next) {
    if(0){
      printf("no next found returning base * segment\n");
    }
    return base_value * segment->values.segment.value;
  }

  double progress = (double)(offset - segment->values.segment.value) /
                    (double)(next->values.segment.start_time - segment->values.segment.start_time);

  gka_apply_transition(
      &progress, segment->values.segment.transition, segment, segment->values.segment.value, next->values.segment.value
  );


  double segment_value =
      segment->values.segment.value + (next->values.segment.value - segment->values.segment.value) * progress;

  if(0){
    printf("no next found running algo\n");
  }
  return base_value * segment_value;
}
