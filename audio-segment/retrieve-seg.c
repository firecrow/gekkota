#include "audio-segment.h"

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

gka_decimal_t gka_get_frame_value_from_event(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const uint32_t rate
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  double value = 0.0;
  int idx = 0;
  while (soundlp) {
    double position = local - start;
    struct gka_entry *s = gka_pointer(blk, soundlp);

    double freq = value_from_segment(blk, s->values.sound.freq, base, position);
    double volume =
        value_from_segment(blk, s->values.sound.volume, base, position);

    if (volume > 1.0 || volume < 0.0) {
      printf("ERROR WITH VOLUME:%lf\n", volume);
      volume = 1.0;
    }

    // reconcile phase for next time
    double step = MAX_PHASE * freq / (double)rate; 

    value = (sin(s->values.sound.phase) * volume);

    s->values.sound.phase += step;

    if (s->values.sound.phase >= MAX_PHASE) {
      s->values.sound.phase -= MAX_PHASE;
    }

    // increment to next sound
    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND);
  }
  return value;
}

gka_decimal_t gka_frame_from_block(struct gka_entry *blk, gka_time_t local, int rate){
  gka_decimal_t frame_value = 0;
  struct gka_entry *head = gka_pointer(blk, 0);
  gka_local_address_t soundlp = head->values.link.addr;
  while (soundlp) {
    struct gka_entry *e = gka_pointer(blk, soundlp);
    if (e->values.event.start > local) {
      continue;
    }
    if (e->values.event.repeat) {
      gka_time_t local_repeat;
      local_repeat = gka_time_modulus(
          local - e->values.event.start, e->values.event.repeat
      );
      frame_value +=
          gka_get_frame_value_from_event(blk, e, 0, local_repeat, rate);
    } else {
      frame_value += gka_get_frame_value_from_event(
          blk, e, e->values.event.start, local, rate
      );
    }

    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND_EVENT);
  }
  return frame_value;
}