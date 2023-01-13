#include "../gekkota.h"

GkaSoundEvent *
Tone::constructSound(double freq, double duration, double repeat_every) {

  gka_segpattern *freq_segs =
      new gka_segpattern(new gka_segment(0.0, freq, &square_ease));

  gka_segpattern *volume_segs =
      new gka_segpattern(new gka_segment(0.0, 0.0, &ease_out));
  volume_segs->add_segment(new gka_segment(BEAT_NS_32nds / 16, 0.2, &ease_in));
  volume_segs->add_segment(new gka_segment(BEAT_NS_32nds * 8, 0.0, &ease_out));

  gka_segpattern *distortion_segs =
      new gka_segpattern(new gka_segment(0.0, 0.0, &square_ease));

  GkaSound *sound = new GkaSound(freq_segs, volume_segs, distortion_segs);

  vector<GkaSound *> group = {sound};

  gka_timeint repeat_time = BEAT_NS_32nds * 64;
  GkaSoundRepeat *repeat = new GkaSoundRepeat(0, repeat_time);

  GkaSoundEvent *eventg = new GkaSoundEvent(group, NULL, repeat);

  return eventg;
}

GkaSoundEvent *Tone::constructKey(double freq, long duration) {
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
}
