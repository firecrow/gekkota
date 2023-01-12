#include "../gekkota.h"

GkaSound::GkaSound(
    gka_segpattern *freq, gka_segpattern *volume, gka_segpattern *distortion
) {
  this->phase = 0.0;
  this->volume_last = 0.0;
  this->freq = freq;
  this->volume = volume;
  this->distortion = distortion;
}

SoundRepeat::SoundRepeat(gka_timeint start, gka_timeint repeat_every) {
  this->start = start;
  this->time = repeat_every;
}

SoundEvent::SoundEvent(
    vector<GkaSound *> soundg, gka_timeint gotime, SoundRepeat *repeat
) {
  this->soundg = soundg;
  this->repeat = repeat;
  this->gotime = gotime;
}

double SoundEvent::getFrameValue(
    gka_timeint start, gka_timeint local, const gka_audio_params &gka_params
) {

  double base = 1.0;
  double value = 0.0;
  for (GkaSound *s : this->soundg) {
    double position = local - start;

    double freq = value_from_segment(s->freq, base, position);
    double volume = value_from_segment(s->volume, base, position);

    if (volume > 1.0 || volume < 0.0) {
      cout << "ERROR WITH VOLUMNE" << volume << endl;
      volume = 1.0;
    }

    double rate = (double)gka_params.rate;
    double step = MAX_PHASE * freq / rate;

    value = (sin(s->phase) * volume);

    s->phase += step;
    if (s->phase >= MAX_PHASE)
      s->phase -= MAX_PHASE;
  }
  return value;
}

void SoundEvent::fadeOut(long position_duration, gka_timeint local) {
  cout << "fade out called" << endl;

  for (GkaSound *s : this->soundg) {
    cout << "updating sound" << endl;
    s->volume->root = new gka_segment(0.0, 0.0, &linear_ease);
  }
}