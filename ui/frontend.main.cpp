#include "../gekkota.h"

using namespace std;

void tear_down(int sig) {
  printf("\x1b[33mtearing down\x1b[0m\n");

  AudioOutputService::running = false;
  MidiService::running = false;
  // FrontEndService::running = false;
  sleep(2);

  MidiService::teardown();
  AudioOutputService::instance.Teardown();
  // FrontEndService::teardown();
  printf("\x1b[33mdown\x1b[0m\n");
}

int main(int argc, char *argv[]) {
  printf("starting.........'\n");

  signal(SIGINT, tear_down);
  Title *title = &Title::instance;

  title->Init();

  struct gka_entry *m = gka_alloc_memblock(64 * GKA_SEGMENT_SIZE);
  Tone::constructSound(m, 440.0, 0.5, BEAT_NS_32nds * 64);
  title->sound_blocks.push_back(m);
  printf(
      "pushing back a block of sounds at %ld %ld\n", m,
      title->sound_blocks.size()
  );
  // test_print_mem_block(m);

  KeysInstrument *Keys = new KeysInstrument(title->midiRouter);
  title->midiRouter->instruments.push_back(Keys);

  struct gka_audio_params gka_params = {
      .output_handle = 0,
      .device = "hw:3,0",
      .rate = 192000,
      .channels = 2,
      .buffer_time = 50000,
      .period_time = 10000,
      .resample = 1,
      .period_event = 0,
      .format = SND_PCM_FORMAT_S32_LE};

  AudioOutputService output = AudioOutputService::instance;

  output.Setup(&gka_params);
  thread audio(output.getAction());

  MidiService::setup();
  thread midi(MidiService::loop);

  /*
  FrontEndService *fe = FrontEndService::getInstance();
  fe->Init();
  fe->commit();
  fe->loop();
  */

  audio.join();
  midi.join();

  printf("done.\n");
}
