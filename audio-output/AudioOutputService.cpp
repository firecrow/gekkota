#include "../gekkota.h"

AudioOutputService::AudioOutputService() {}

AudioOutputService _audiosvc;
AudioOutputService &AudioOutputService::instance = {_audiosvc};

bool AudioOutputService::running = 1;

void AudioOutputService::Setup(struct gka_audio_params *gka_params) {
  setup_hw(gka_params);
  this->gka_params = *gka_params;
};

function<void(void)> AudioOutputService::getAction() {
  const struct gka_audio_params &params = {this->gka_params};
  function<void(void)> f = [params]() {
    cout << "running output loop" << endl;
    write_loop(params);
    cout << "end audio loop" << endl;
  };
  return f;
};

void AudioOutputService::Teardown() {
  cout << "tearing down audio" << endl;
  tear_down_audio(&(this->gka_params));
};
