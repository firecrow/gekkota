class Title {

public:
  Title();
  void Init();
  static Title &instance;
  gka_time_t start_time;
  Logger logger;
  InstrumentRouter *midiRouter;
  vector<struct gka_mem_block *> sound_blocks;
};
