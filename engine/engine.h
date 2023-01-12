#include <alsa/asoundlib.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// clang-format off
#define GkaSetAlsaHw(FUNC) \
  do { \
  int r = (FUNC); \
  if(r < 0) \
    fprintf(stderr, "Error setting up alsa hardware params %s, %s:%d", snd_strerror(r), __FILE__, __LINE__); \
} while (0) \
// clang-format on

static snd_pcm_sframes_t buffer_size;
static snd_pcm_sframes_t period_size;
static snd_output_t *output = NULL;

struct gka_output_objects {
  snd_pcm_channel_area_t *areas;
  signed short *samples;
};

struct gka_audio_params {
  snd_pcm_t *output_handle;
  char device[128];
  unsigned int rate;
  unsigned int channels;
  unsigned int buffer_time;
  unsigned int period_time;
  int verbose;
  int resample;
  int period_event;
  snd_pcm_format_t format;
  snd_pcm_sframes_t buffer_size;
  snd_pcm_sframes_t period_size;
};

int set_hwparams(
    struct gka_audio_params *gka_params, snd_pcm_t *handle,
    snd_pcm_hw_params_t *params, snd_pcm_access_t access
);

int set_swparams(
    struct gka_audio_params *gka_params, snd_pcm_t *handle,
    snd_pcm_sw_params_t *swparams
);

void tear_down_audio(struct gka_audio_params *output_objects);
int setup_hw(struct gka_audio_params *gka_params);
int write_loop(
    const struct gka_audio_params &gka_params
);
