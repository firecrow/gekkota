#include <alsa/asoundlib.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <functional>
#include <iostream>

extern "C" {
#include "sound/sound.h"
#include "error/error.h"
}

using namespace std;

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

class Engine {
  public:
  Engine();
  static Engine &instance;
  vector <RenderHandler *> handlers;
  double *render(vector<struct gka_mem_block *>blocks, int count, uint32_t rate);
};

class RenderHandler {
public:
  virtual function<void(void)> getAction(gka_time_t elapsed);
  virtual RenderHandler *makeInstance(
    struct gka_mem_block *src, int count, int rate
  );
  struct gka_mem_block *src;
  gka_decimal_t *dest;
  int count;
  int rate;
};

class HostRenderHandler: public RenderHandler
{
public:
  HostRenderHandler();
  RenderHandler *makeInstance(
    struct gka_mem_block *src, int count, int rate
  );
  function<void(void)> getAction(gka_time_t elapsed);
  ~HostRenderHandler(){
    delete this->dest;
  }
};

class RenderFinalizer 
{
public:
  RenderFinalizer(int count){
    this->dest = (double *)malloc(sizeof(gka_decimal_t) * count);
    this->count = count;
    if (!this->dest) {
      memory_error(
          GKA_GLOBAL_MEMORY_ERROR,
          "Error allocating destination array of doubles in render finalizer\n"
      );
    }
  };
  gka_decimal_t *dest;
  int count;
  RenderHandler *marryToDest(gka_decimal_t *src){
    for(int i = 0; i < count; i ++){
      dest[i] += src[i];
    }
  }
};