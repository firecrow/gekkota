struct Tone {
  static GkaSoundEvent *
  constructSound(double freq, double duration, double repeat_every);
  static GkaSoundEvent *constructKey(double freq, long duration);
};
