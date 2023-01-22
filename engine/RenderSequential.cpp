#include "engine.h"

HostRenderHandler::HostRenderHandler(struct gka_mem_block *blk, int count) {
  this->count = count;
  this->src = blk;
  this->dest = (double *)malloc(sizeof(double) * count);
  if (!this->dest) {
    memory_error(
        GKA_GLOBAL_MEMORY_ERROR,
        "Error allocating destination array of doubles\n"
    );
  }
}

function<void(void)> HostRenderHandler::getAction() {
  return [dest = this->dest, src = this->src]() {

  };
};