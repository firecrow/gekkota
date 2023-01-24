struct Tone {
  static gka_local_address_t constructSound(
      struct gka_entry *blk, double freq, gka_time_t duration,
      gka_time_t repeat_every
  );
  static gka_local_address_t
  constructKey(struct gka_entry *blk, double freq, gka_time_t duration);
};
