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
}

void KeysInstrument::closeNote(gka_midi_event *event) {
  cout << "key up" << (int)event->key << endl;
}