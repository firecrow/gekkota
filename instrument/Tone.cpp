#include "../gekkota.h"

gka_local_address_t Tone::constructSound(
    struct gka_mem_block *blk, double freq, gka_time_t duration,
    gka_time_t repeat_every
) {

  // frequency
  gka_local_address_t freq_pattern_segs =
      gka_segment_new(blk, 0, freq, GKA_CLIFF);

  // volume
  gka_local_address_t vol_pattern_segs =
      gka_segment_new(blk, 0, 0.0, GKA_LINEAR);

  gka_segpattern_add_segment_values(
      blk, vol_pattern_segs, BEAT_NS_32nds / 16, 0.2, GKA_EASE_IN
  );
  gka_segpattern_add_segment_values(
      blk, vol_pattern_segs, BEAT_NS_32nds * 8, 0.0, GKA_EASE_OUT
  );
  // sound
  gka_local_address_t soundlp =
      gka_sound_create(blk, freq_pattern_segs, vol_pattern_segs);

  // sound event
  gka_local_address_t eventlp =
      gka_sound_event_create(blk, soundlp, 0, repeat_every);

  return eventlp;
}

gka_local_address_t Tone::constructKey(
    struct gka_mem_block_t *blk, double freq, gka_time_t duration
) {
  return 0;
  /*
  double fluctuate_tone = 3.0;
  vector<GkaSound *> group = {};
  int GROUP_SIZE = 1;
  for (int i = 0; i < GROUP_SIZE; i++) {
  double local_freq =
      freq + fluctuate_tone * (rand() / RAND_MAX) - (fluctuate_tone * 0.5);

  gka_segpattern *freq_segs =
      new gka_segpattern(new gka_segment(0, local_freq, &square_ease));

  gka_segpattern *volume_segs =
      new gka_segpattern(new gka_segment(0, 0.1 / GROUP_SIZE, &ease_out));

  gka_segpattern *distortion_segs =
      new gka_segpattern(new gka_segment(0, 0.0, &square_ease));

  GkaSound *sound = new GkaSound(freq_segs, volume_segs, distortion_segs);
  sound->phase = (rand() / RAND_MAX) * MAX_PHASE;
  group.push_back(sound);
  }

  GkaSoundEvent *eventg = new GkaSoundEvent(group, NULL, NULL);

  return eventg;
  */
}
