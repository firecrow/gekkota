struct Tone {
  static SoundEvent *
  constructSound(double freq, double duration, double repeat_every);
  static SoundEvent *constructKey(double freq, long duration);
};
