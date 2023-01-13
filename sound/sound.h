#define MAX_PHASE 2. * M_PI

struct GkaSound {
  struct gka_segpattern *freq;
  struct gka_segpattern *volume;
  struct gka_segpattern *distortion;
  double phase;
  double step;
  double volume_last;

  GkaSound(
      gka_segpattern *freq, gka_segpattern *volume, gka_segpattern *distortion
  );
};

struct GkaSoundRepeat {
  gka_timeint start;
  gka_timeint time;
  GkaSoundRepeat(gka_timeint start, gka_timeint repeat_every);
};

struct GkaSoundEvent {
  vector<GkaSound *> soundg;
  gka_timeint gotime;
  GkaSoundRepeat *repeat;

  GkaSoundEvent(
      vector<GkaSound *> soundg, gka_timeint gotime, GkaSoundRepeat *repeat
  );
  GkaSoundEvent(vector<GkaSound *> soundg, gka_timeint gotime);
  double getFrameValue(gka_timeint start, gka_timeint local, const long rate);
  void fadeOut(long position_duration, gka_timeint local);
};
