#define STR_ID_LENGTH 256

typedef double instrument_values[256];

struct Instrument {
  int id;
  char name[STR_ID_LENGTH];
  instrument_values keys;
  virtual void onMidi(gka_midi_event *event);
};

struct InstrumentRouter {
  vector<SoundEvent *> events;
  virtual void submit(SoundEvent *sound);
  virtual void onMidi(gka_midi_event *event);
  vector<Instrument *> instruments;
};

struct KeysInstrument : Instrument {
  vector<SoundEvent *> events;
  Instrument *instrument;
  InstrumentRouter *router;
  virtual void onMidi(gka_midi_event *event);
  virtual void openNote(gka_midi_event *event);
  virtual void closeNote(gka_midi_event *event);
  SoundEvent *sound_by_key[128];

  KeysInstrument(InstrumentRouter *router);
};

struct Classical {
  static instrument_values note_frequency;
};
