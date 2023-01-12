#define MAX_PHASE 2. * M_PI

struct GkaSound {
  struct gka_segpattern *freq;
  struct gka_segpattern *volume;
  struct gka_segpattern *distortion;
  double phase;
  double volume_last;

  GkaSound(
      gka_segpattern *freq, gka_segpattern *volume, gka_segpattern *distortion
  );
};

struct SoundRepeat {
  gka_timeint start;
  gka_timeint time;
  SoundRepeat(gka_timeint start, gka_timeint repeat_every);
};

struct SoundEvent {
  vector<GkaSound *> soundg;
  gka_timeint gotime;
  SoundRepeat *repeat;

  SoundEvent(
      vector<GkaSound *> soundg, gka_timeint gotime, SoundRepeat *repeat
  );
  double getFrameValue(
      gka_timeint start, gka_timeint local, const gka_audio_params &gka_params
  );
  void fadeOut(long position_duration, gka_timeint local);
};
