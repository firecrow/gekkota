class Title {

public:
  Title();
  void Init();
  static Title &instance;
  gka_timeint start_time;
  Logger logger;
  InstrumentRouter *midiRouter;
  vector<struct gka_mem_block *> sound_blocks;
};
