#include "../gekkota.h"

void dispatch_to_instrument(struct gka_midi_event *event) {
  if (event->action == (char)144) {
    double note = Classical::note_frequency[event->key];
    Title::instance.midiRouter->onMidi(event);
  } else if (event->action == (char)128) {
    Title::instance.midiRouter->onMidi(event);
  }
}
