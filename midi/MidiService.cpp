#include "../gekkota.h"

bool MidiService::running = 1;

void MidiService::setup() { cout << "setting up midi " << endl; };

void MidiService::teardown() { cout << "tearing down midi " << endl; };

void MidiService::loop() {
  midi_accept();
  cout << "midi ended" << endl;
};