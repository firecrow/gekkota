#include <alsa/asoundlib.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <vector>

#include <functional>
#include <iostream>

extern "C" {
#include "error/error.h"
}

#include "audio-segment/audio-segment.h"

using namespace std;

/**
 * This is a wrapper macro used to place line and file information into error
 * messages
 */
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

/**
 * These are parameters set as configuration but responsive to the hardware
 * profile becuase some hardware will render at different rate or periods, this
 * struct is used to communicate those changes to the engine code that processes
 * the frames
*/
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

/**
 * This function sets up the hardware parameters and handles for the audio output
*/
int set_hwparams(
    struct gka_audio_params *gka_params, snd_pcm_t *handle,
    snd_pcm_hw_params_t *params, snd_pcm_access_t access
);

/**
 * This sets up the sofware parameters used by the ALSA userspace library
*/
int set_swparams(
    struct gka_audio_params *gka_params, snd_pcm_t *handle,
    snd_pcm_sw_params_t *swparams
);

/**
 * This manages cleanup and closing the hardware device (ALSA)
*/
void tear_down_audio(struct gka_audio_params *output_objects);

/**
 * This function manages opening the hardware device for sound output (ALSA)
*/
int setup_hw(struct gka_audio_params *gka_params);

/**
 * This serves as an entry point into running the audio gathering of sound
 * segments and processing engine to generate dat to send to the audio output
 * 
 * TOTO: This is going to be centralized into the engine more directly going
 * forward
*/
int write_loop(
    const struct gka_audio_params &gka_params
);

/**
 * This class holds the final frame information as the last step in generating the audio data through the engine
 * 
 * it is a gathering point between the the handlers (GPU or HOST) to allow for multiple methods of rendering
*/
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
  void marryToDest(gka_decimal_t *src){
    for(int i = 0; i < count; i ++){
      dest[i] += src[i];
    }
  }
};

/**
 * This serves as a base class for all the render handlers which by implementing
 * this interface, make themselves available to be included in processing sound
 * frames
*/
class RenderHandler {
public:
  virtual void render(gka_time_t elapsed){};
  virtual RenderHandler *makeInstance(
    struct gka_entry *src, int count, int rate
  ){};
  struct gka_entry *src;
  gka_decimal_t *dest;
  int count;
  int rate;
};

/**
 * This class serves as the entry point and cooridnator of the rendering process
*/
class Engine {
  public:
  Engine();
  static Engine &instance;
  vector <RenderHandler *> handlers;
  void render(RenderFinalizer *finalizer, vector<struct gka_entry *>blocks, int count, uint32_t rate);
};

/**
 * This handler manages processing of audio data on the host (CPU) and can
 * implement everything the GPU handler does with less parallelism
 * 
 * It is capable of generating frames from the mem block entry/segment structure of
 * sound and sound events otherwise sent to the GPU
*/
class HostRenderHandler: public RenderHandler
{
public:
  HostRenderHandler();
  RenderHandler *makeInstance(
    struct gka_entry *src, int count, int rate
  );
  void render(gka_time_t elapsed);
  ~HostRenderHandler(){
    free(this->dest);
  }
};

/**
 * This is the handler for processing/generating sound data on the GPU
 * 
 * It accepts the mem block entry/segment memory structures and processes the
 * steps and phases required for generating final frames, some of this memory
 * persists on the GPU during this process and is returned with the full period
 * 
 * all memory is free'd between generation of periods
*/
class HipDeviceRenderHandler: public RenderHandler
{
public:
  HipDeviceRenderHandler();
  RenderHandler *makeInstance(
    struct gka_entry *src, int count, int rate
  );
  void render(gka_time_t elapsed);
  ~HipDeviceRenderHandler(){
    free(this->dest);
  }
};
