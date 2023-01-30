#include "../gekkota.h"

Title::Title() {
  this->sound_blocks = {};
  this->midiRouter = new InstrumentRouter();
  this->start_time = 0;
}
void Title::Init() { this->start_time = gka_now(); }

Title inst;
Title &Title::instance = {inst};