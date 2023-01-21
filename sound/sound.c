#include <math.h>
#include "sound.h"

gka_local_address_t gka_sound_create(
    struct gka_mem_block *blk, gka_local_address_t freq, gka_local_address_t volume
) {

  gka_local_address_t localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE);

  if (localp == GKA_BOUNDRY_ACTION) {
    fprintf(stderr, "Error allocating segment %s:%d\n", __FILE__, __LINE__);
    return GKA_MEMORY_FAILURE;
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.sound.freq = freq;
  s->values.sound.volume = volume;
  s->values.all.type = GKA_SOUND;

  return localp;
};

gka_local_address_t gka_sound_event_place(struct gka_mem_block *blk);

gka_local_address_t gka_sound_event_create(
    struct gka_mem_block *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
) {
  gka_local_address_t localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE);

  if (localp == GKA_BOUNDRY_ACTION) {
    fprintf(stderr, "Error allocating segment %s:%d\n", __FILE__, __LINE__);
    return GKA_MEMORY_FAILURE;
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.event.start = start;
  s->values.event.repeat = repeat;
  s->values.event.sounds = sounds;
  s->values.all.type = GKA_SOUND_EVENT;

  return localp;
}

gka_value_t gka_get_frame_value_from_event(
    struct gka_mem_block *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const long rate
) {

  gka_local_address_t soundlp = event->values.event.sounds;
  double base = 1.0;
  double value = 0.0;
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

    value = (sin(s->values.sound.phase) * volume);

    // reconcile phase for next time
    s->values.sound.step = MAX_PHASE * freq / (double)rate;
    s->values.sound.phase += s->values.sound.step;
    if (s->values.sound.phase >= MAX_PHASE) {
      s->values.sound.phase -= MAX_PHASE;
    }

    // increment to next sound
    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND);
  }
  return value;
}