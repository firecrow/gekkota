#include "../gekkota.h"

Title::Title() {
  printf("\x1b[31mTitle inst x2\x1b[0m\n");
  this->sound_blocks = {};
  this->midiRouter = new InstrumentRouter();
  this->start_time = 0;
}
void Title::Init() { this->start_time = gka_now(); }

Title inst;
Title &Title::instance = {inst};