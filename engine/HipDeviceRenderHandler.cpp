#include "gekkota.h"
#include "audio-segment/hip-calculations-process-audio.hpp"

HipDeviceRenderHandler::HipDeviceRenderHandler() {}
RenderHandler *HipDeviceRenderHandler::makeInstance(
    struct gka_entry *src, int count, int rate
) {
  HipDeviceRenderHandler *inst = new HipDeviceRenderHandler();
  inst->dest = (double *)malloc(sizeof(double) * count);
  if (!inst->dest) {
    memory_error(
        GKA_GLOBAL_MEMORY_ERROR,
        "Error allocating destination array of doubles for hip device\n"
    );
  }
  inst->src = src;
  inst->count = count;
  inst->rate = rate;
  return inst;
}

void HipDeviceRenderHandler::render(gka_time_t elapsed) {
  if (!this->count || this->src == nullptr) {
    initilization_error(
        GKA_GLOBAL_INIT_ERROR,
        "data required for rendering not yet sent to render handler\n"
    );
    return;
  }
  gka_process_audio_hip(
      this->dest, this->src, this->count, this->rate, elapsed
  );
}