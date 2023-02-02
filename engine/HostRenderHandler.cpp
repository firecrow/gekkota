#include "gekkota.h"
#include "audio-segment/host-calculations.hpp"

HostRenderHandler::HostRenderHandler() {}
RenderHandler *
HostRenderHandler::makeInstance(struct gka_entry *src, int count, int rate) {
  HostRenderHandler *inst = new HostRenderHandler();
  inst->dest = (double *)malloc(sizeof(double) * count);
  if (!inst->dest) {
    memory_error(
        GKA_GLOBAL_MEMORY_ERROR,
        "Error allocating destination array of doubles\n"
    );
  }
  inst->src = src;
  inst->count = count;
  inst->rate = rate;
  return inst;
}

void HostRenderHandler::render(gka_time_t elapsed) {
  if (!this->count || this->src == nullptr) {
    initilization_error(
        GKA_GLOBAL_INIT_ERROR,
        "data required for rendering not yet sent to render handler\n"
    );
    return;
  }
  gka_time_t local;
  for (int f = 0; f < count; f++) {
    local = elapsed + f;

    double frame_value = 0.0;
    int sound_id = 0;

    dest[f] = gka_frame_from_block(this->src, local, this->rate);
  }
};