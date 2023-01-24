#include <math.h>
#include "sound.h"

gka_local_address_t gka_sound_create(
    struct gka_entry *blk, gka_local_address_t freq, gka_local_address_t volume
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

gka_local_address_t gka_sound_event_place(struct gka_entry *blk);

gka_local_address_t gka_sound_event_create(
    struct gka_entry *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
) {

  gka_local_address_t localp = 0;

  struct gka_entry *head = gka_pointer(blk, 0);
  // this is the first sound becuase the entry record does not yet link to a starting place
  if(head->values.all.type == GKA_UNSPECIFIED){
    head->values.all.type = GKA_NEXT_LOCAL;

    localp = gka_allocate_space(blk, GKA_SEGMENT_SIZE*2);
    if (localp == GKA_BOUNDRY_ACTION) {
      fprintf(stderr, "Error allocating segment %s:%d\n", __FILE__, __LINE__);
      return GKA_MEMORY_FAILURE;
    }

    head->values.link.addr = localp;

    gka_local_address_t neighbour_address = gka_next_local(blk, localp);
    gka_set_entry_status(blk, neighbour_address, GKA_RESERVED_BY_NEIGHBOUR);


  }else if(head->values.all.type == GKA_NEXT_LOCAL){
    printf("\x1b[36mnth from %ld\x1b[0m\n", head->values.link.addr/GKA_SEGMENT_SIZE);
    localp = gka_add_entry_to_set(blk, head->values.link.addr, GKA_SOUND_EVENT);
  }

  struct gka_entry *s = (struct gka_entry *)gka_pointer(blk, localp);
  s->values.event.start = start;
  s->values.event.repeat = repeat;
  s->values.event.sounds = sounds;
  s->values.all.type = GKA_SOUND_EVENT;

  return localp;
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