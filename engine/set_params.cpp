#include "engine.h"

int set_hwparams(
    struct gka_audio_params *gka_params, snd_pcm_t *handle,
    snd_pcm_hw_params_t *params, snd_pcm_access_t access
) {
  unsigned int rrate;
  snd_pcm_uframes_t size;
  int dir;

  GkaSetAlsaHw(snd_pcm_hw_params_any(handle, params));
  GkaSetAlsaHw(
      snd_pcm_hw_params_set_rate_resample(handle, params, gka_params->resample)
  );
  GkaSetAlsaHw(snd_pcm_hw_params_set_access(handle, params, access));
  GkaSetAlsaHw(snd_pcm_hw_params_set_format(handle, params, gka_params->format)
  );
  GkaSetAlsaHw(
      snd_pcm_hw_params_set_channels(handle, params, gka_params->channels)
  );

  rrate = gka_params->rate;
  GkaSetAlsaHw(snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0));
  if (rrate != gka_params->rate) {
    printf(
        "Different rate returned than requested (requested %uHz, get %iHz)\n",
        gka_params->rate
    );
    return -EINVAL;
  }

  GkaSetAlsaHw(snd_pcm_hw_params_set_buffer_time_near(
      handle, params, &(gka_params->buffer_time), &dir
  ));
  GkaSetAlsaHw(snd_pcm_hw_params_get_buffer_size(params, &size));
  gka_params->buffer_size = size;
  GkaSetAlsaHw(snd_pcm_hw_params_set_period_time_near(
      handle, params, &(gka_params->period_time), &dir
  ));

  GkaSetAlsaHw(snd_pcm_hw_params_get_period_size(params, &size, &dir));
  gka_params->period_size = size;

  GkaSetAlsaHw(snd_pcm_hw_params(handle, params));

  return 0;
}

int set_swparams(
    struct gka_audio_params *gka_params, snd_pcm_t *handle,
    snd_pcm_sw_params_t *swparams
) {

  GkaSetAlsaHw(snd_pcm_sw_params_current(handle, swparams));
  GkaSetAlsaHw(snd_pcm_sw_params_set_start_threshold(
      handle, swparams,
      (gka_params->buffer_size / gka_params->period_size) *
          gka_params->period_size
  ));
  GkaSetAlsaHw(snd_pcm_sw_params_set_avail_min(
      handle, swparams,
      gka_params->period_event ? gka_params->buffer_size
                               : gka_params->period_size
  ));
  if (gka_params->period_event) {
    GkaSetAlsaHw(snd_pcm_sw_params_set_period_event(handle, swparams, 1));
  }
  GkaSetAlsaHw(snd_pcm_sw_params(handle, swparams));
  return 0;
}
