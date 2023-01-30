#include "../gekkota.h"

gka_local_address_t Tone::constructSound(
    struct gka_entry *blk, double freq, gka_time_t duration,
    gka_time_t repeat_every
) {

  // frequency
  gka_local_address_t freq_pattern_segs =
      gka_segment_new(blk, 0, freq, GKA_CLIFF);

  // volume
  gka_local_address_t vol_pattern_segs =
      gka_segment_new(blk, 0, 0.0, GKA_EASE_IN);

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

gka_local_address_t
Tone::constructKey(struct gka_entry *blk, double freq, gka_time_t duration) {
  return 0;
}
