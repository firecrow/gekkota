#include "audio-segment.h"

struct gka_entry *
gka_segment_from_pattern(struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t offset) {
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
    struct gka_entry *blk, gka_local_address_t current, double base_value, gka_time_t offset
) {

  struct gka_entry *segment = gka_segment_from_pattern(blk, current, offset);
  struct gka_entry *next = NULL;

  if(segment->values.segment.start_time < offset){
    gka_local_address_t nextlp = gka_entry_next(blk, gka_to_local(blk, segment), GKA_SEGMENT_VALUE);
    if(nextlp){
      next = gka_pointer(blk, nextlp);
    }
  }

  if (!next) {
    return base_value * segment->values.segment.value;
  }

  double progress = (double)(offset - segment->values.segment.start_time) /
                    (double)(next->values.segment.start_time - segment->values.segment.start_time);

  gka_apply_transition(
      &progress, segment->values.segment.transition, segment, segment->values.segment.value, next->values.segment.value
  );


  double segment_value =
      segment->values.segment.value + ((next->values.segment.value - segment->values.segment.value) * progress);

  return base_value * segment_value;
}

int gka_count_sounds_in_block(struct gka_entry *blk) {

  int count = 0;
  struct gka_entry *head = gka_pointer(blk, 0);
  gka_local_address_t soundlp = head->values.head.addr;

  while (soundlp) {
    struct gka_entry *e = gka_pointer(blk, soundlp);
    count++;
    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND_EVENT);
  }
  return count;
}
