class Title {

public:
  Title();
  void Init();
  static Title &instance;
  gka_timeint start_time;
  Logger logger;
  InstrumentRouter *midiRouter;
  vector<GkaSoundEvent *> sound_events;
};
