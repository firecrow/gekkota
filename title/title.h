class Title {

  /**
   * Title represents the current state of the track/song being played or
   * processed, it is globally accessed by the engine to delegate the processing
   * of it's data which is then sent to the audio output
   *
   * midi events from MIDI hardware also access this object to update what
   * sounds/effects to play
   *
   * TODO: build more consistent access policies so that agumenting the segments
   * can be done in parallel
   */
public:
  Title();
  void Init();
  static Title &instance;
  gka_time_t start_time;
  Logger logger;
  /**
   * This is the router object that accepts MIDI data and aguments the sound
   * blocks from those actions
   */
  InstrumentRouter *midiRouter;
  /**
   * This represents all of the sound composition data for this song/track, this
   * is the block which is sent to the engine for processing on the Host or GPU
   * before being sent to the audio output
   */
  vector<struct gka_entry *> sound_blocks;
};
