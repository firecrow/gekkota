#include "engine.h"
#include "gekkota.h"

HostRenderHandler::HostRenderHandler() {}
RenderHandler *HostRenderHandler::makeInstance(
    struct gka_mem_block *src, int count, int rate
) {
  HostRenderHandler *inst = new HostRenderHandler();
  inst->dest = (double *)malloc(sizeof(double) * count);
  if (!inst->dest) {
    memory_error(
        GKA_GLOBAL_MEMORY_ERROR,
        "Error allocating destination array of doubles\n"
    );
  }
  this->src = src;
  this->count = count;
  this->rate = rate;
  return inst;
}

function<void(void)> HostRenderHandler::getAction(gka_time_t elapsed) {
  if (!this->count || this->src == nullptr) {
    initilization_error(
        GKA_GLOBAL_INIT_ERROR,
        "data required for rendering not yet sent to render handler\n"
    );
    return nullptr;
  }
  return [count = this->count, dest = this->dest, src = this->src,
          rate = this->rate, elapsed]() {
    gka_time_t local;
    for (int f = 0; f < count; f++) {
      local = elapsed + f;

      double frame_value = 0.0;
      int sound_id = 0;

      dest[f] = gka_frame_from_block(src, local, rate);
    }
  };
};