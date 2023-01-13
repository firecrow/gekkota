#include "../gekkota.h"

void InstrumentRouter::submit(GkaSoundEvent *sound) {
  Title::instance.sound_events.push_back(sound);
}

void InstrumentRouter::onMidi(gka_midi_event *event) {
  for (Instrument *m : this->instruments) {
    m->onMidi(event);
  }
}