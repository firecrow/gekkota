#include "audio-segment.h"

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