#include "../gekkota.h"

KeysInstrument::KeysInstrument(InstrumentRouter *router) {
  this->router = router;
}

void KeysInstrument::onMidi(gka_midi_event *event) {
  if (event->action == (char)144) { // key down
    this->openNote(event);
  } else if (event->action == (char)128) { // key up
    this->closeNote(event);
  }
}

void KeysInstrument::openNote(gka_midi_event *event) {
  cout << "key down " << (int)event->key << endl;
  double note = Classical::note_frequency[event->key];
  GkaSoundEvent *tone = Tone::constructKey(note, BEAT_NS);
  this->sound_by_key[event->key] = tone;
  this->router->submit(tone);
}

void KeysInstrument::closeNote(gka_midi_event *event) {
  cout << "key up" << (int)event->key << endl;
  double note = Classical::note_frequency[event->key];
  GkaSoundEvent *sound = this->sound_by_key[event->key];
  if (!sound) {
    return;
  }

  gka_timeint elapsed = gka_now() - Title::instance.start_time;

  sound->fadeOut(BEAT_NS_32nds, elapsed);
}