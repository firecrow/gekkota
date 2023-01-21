struct Tone {
  static gka_local_address_t constructSound(
      struct gka_mem_block *blk, double freq, gka_time_t duration,
      gka_time_t repeat_every
  );
  static gka_local_address_t
  constructKey(struct gka_mem_block_t *blk, double freq, gka_time_t duration);
};
