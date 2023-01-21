struct Tone {
  static gka_local_address_t constructSound(
      struct gka_mem_block_t *blk, double freq, double duration,
      double repeat_every
  );
  static gka_local_address_t
  constructKey(struct gka_mem_block_t *blk, double freq, long duration);
};
