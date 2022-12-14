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
  Title title = Title::instance;

  title.Init();

  SoundEvent *tone = Tone::constructSound(440.0, 0.5, 2.0);
  title.sound_events.push_back(tone);

  KeysInstrument *Keys = new KeysInstrument(title.midiRouter);
  title.midiRouter->instruments.push_back(Keys);

  struct gka_audio_params gka_params = {
      .output_handle = 0,
      .device = "hw:3,0",
      .rate = 96000,
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
