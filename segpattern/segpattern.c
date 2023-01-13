#include "../gekkota.h"

void linear_ease(
    double *progress, struct gka_segment *segment, double start, double end
) {}

void square_ease(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress = start;
}

void ease_in(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress *= (*progress);
}

void ease_flip(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress = 1 - *progress;
}

void ease_out(
    double *progress, struct gka_segment *segment, double start, double end
) {
  double base = 1 - *progress;
  double curved = base * base;
  *progress *= 1 - curved;
}

void ease_inout(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress *= (*progress);
}

int gka_segment_create(
        struct gka_mem_block *blk, gka_value gotime, gka_decimal start_value,
        gka_subvalue ease
    ) {
  gka_local_address localp =
      gka_allocate_space(blk, sizeof(struct gka_segment));

  if (localp == GKA_INVALID_MEMORY) {
    fprintf(stderr, "Error allocating segment");
    return GKA_MEMORY_FAILURE;
  }

  struct gka_segment *s = (gka_segment *)gka_pointer(blk, localp);
  seg->gotime = gotime;
  seg->start_value = start_value;
  seg->head.head_op.transition = ease;

  return GKA_SUCCESS;
}

gka_local_address gka_segpattern_get_next_segment(struct gka_mem_block *blk, gka_local_address local_seg){
  struct gka_entry *next = gka_pointer(blk, seg+sizeof(struct gka_entry));
  if(seg->head->operand == GKA_SEGMENT_VALUE){
    return next;
  }else if(seg->head->operand == GKA_NEXT_BLOCK){
    return seg->head.head_op.entry_on_next_block;
  }
  return GKA_BOUNDRY_ACTION;
}

struct gka_segment *gka_segment_getnext(struct gka_segment *current){
  gka_local_address seglp = gka_segpattern_get_next_segment(currentl);
}

int gka_create_link(struct gka_mem_block *blk, gka_local_address place){
  struct gka_entry *entry = gka_pointer(blk, place);
  if(entry->head.operand != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER");
    exit(1);
    return 0;
  }
  gka_local_address newlp = gka_allocate_space(blk, sizeof(struct gka_segment));
  entry->head.operand = GKA_NEXT_LOCAL;
  entry->head.head_op.entry_on_next_block = newlp;


  return newlp;
}

int gka_extend_pattern(pattern, current, seg){
  struct gka_entry *neighbour = get_pointer(pattern->blk, current+sizeof(struct gka_entry));
  struct gka_entry *next_neighbour = get_pointer(pattern->blk, current+sizeof(struct gka_entry)*2);
  if(neighbour->head.operand != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER");
    exit(1);
    return 0;
  }
  if(next_neighbour->head.operand == GKA_UNSPECIFIED){
    gka_local_address newlp = gka_segment_create(pattern->blk, seg->gotime, seg->start_value, seg->ease);
    next_neighbour->head.operand = GKA_RESERVED_BY_NEIGHBOUR;
  }else{
    gka_local_address newlp = gka_segment_create_link(pattern->blk, current);
    next_neighbour = get_pointer(pattern->blk, newlp+sizeof(struct gka_entry));
    next_neighbour->head.operand = GKA_RESERVED_BY_NEIGHBOUR;

    struct gka_segment *s = (gka_segment *)gka_pointer(blk, newlp);

    s->gotime = s->gotime;
    s->start_value = s->start_value;
    s->head.head_op.transition = s->ease;
  }
}

void gka_segpattern_add_segment(struct gka_segpatern *pattern, struct gka_segment *seg) {
  if (root == NULL) {
    root = seg;
  } else {
    gka_segment *next = pattern->root;
    gka_segment *current = next;
    while (next) {
      current = next;
      next = gka_segment_getnext(pattern->blk, next - pattern->blk);
    }
    gka_extend_pattern(pattern, current, seg);
  }
}

struct gka_segment *
segment_from_segment(struct gka_segpattern *pattern, gka_timeint offset) {
  struct gka_segment *segment = pattern->root;

  next = gka_segment_getnext(pattern->blk, pattern->root - pattern->blk);
  while (segment && next && (offset > next->gotime)) {
    segment = gka_segment_getnext(pattern->blk, next - pattern->blk);
  }
  return segment;
}

double value_from_segment(
    struct gka_segpattern *pattern, double base_value, gka_timeint offset
) {

  struct gka_segment *segment = segment_from_segment(pattern, offset);

  double next_value = segment->start_value;
  if (segment && segment->next) {
    next_value = segment->next->start_value;
  }

  if (!segment->next) {
    return base_value * segment->start_value;
  }

  double progress = (double)(offset - segment->gotime) /
                    (double)(segment->next->gotime - segment->gotime);

  segment->ease(
      &progress, segment, segment->start_value, segment->next->start_value
  );

  // debug output
  if (0) {
    printf(
        "\x1b[34moffset %d,progress %lf, between %d/%lf and %d/%lf\x1b[0m",
        offset, progress, segment->gotime, segment->start_value,
        segment->next->gotime, segment->next->start_value
    );
  }

  double segment_value =
      segment->start_value + (next_value - segment->start_value) * progress;

  return base_value * segment_value;
}
