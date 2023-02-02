#define STR_ID_LENGTH 256

/**
 * A number of MIDI 2.0 interfaces use 0..127 values so this is a type capable
 * of holding keys and other relevant MIDI event information
 */
typedef double instrument_values[256];

/**
 * This class is a general interface for any instrument
 */
struct Instrument {
  int id;
  char name[STR_ID_LENGTH];
  instrument_values keys;
  virtual void onMidi(gka_midi_event *event);
};

/**
 * This is the router which accepts MIDI information and propagates int down
 * into the sound data used by the engine
 */
struct InstrumentRouter {
  // vector<GkaSoundEvent *> events;
  // virtual void submit(GkaSoundEvent *sound);
  virtual void onMidi(gka_midi_event *event);
  vector<Instrument *> instruments;
};

/**
 * This is an instrument spcecific to handling MIDI musical keyboard events
 */
struct KeysInstrument : Instrument {
  // vector<GkaSoundEvent *> events;
  Instrument *instrument;
  InstrumentRouter *router;
  virtual void onMidi(gka_midi_event *event);
  virtual void openNote(gka_midi_event *event);
  virtual void closeNote(gka_midi_event *event);
  // GkaSoundEvent *sound_by_key[128];

  KeysInstrument(InstrumentRouter *router);
};

/**
 * This is the class which, holds the frequency numbers for notes in the real
 * world
 */
struct Classical {
  static instrument_values note_frequency;
};
