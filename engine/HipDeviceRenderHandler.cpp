#include "gekkota.h"
#include <hip/hip_runtime.h>
#include "hip-device/hip-device-mem-blocks.c"

__global__ void gkaHipProcessBlock(
    gka_decimal_t *dest, struct gka_entry *src, gka_time_t elapsed, int rate
) {
  int frameId = hipThreadIdx_x + hipBlockDim_x * hipBlockIdx_x;
  gka_time_t local = elapsed + frameId;
  dest[frameId] = gka_frame_from_block_hipdevice(src, frameId, local, rate);
}

HipDeviceRenderHandler::HipDeviceRenderHandler() {}
RenderHandler *HipDeviceRenderHandler::makeInstance(
    struct gka_mem_block *src, int count, int rate
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

function<void(void)> HipDeviceRenderHandler::getAction(gka_time_t elapsed) {
  if (!this->count || this->src == nullptr) {
    initilization_error(
        GKA_GLOBAL_INIT_ERROR,
        "data required for rendering not yet sent to render handler\n"
    );
    return nullptr;
  }
  return [count = this->count, dest = this->dest, src = this->src,
          rate = this->rate, elapsed]() {
    unsigned blockSize = 256;

    struct gka_entry *srcBuff;
    double *destBuff;

    // printf("allocated a src buf of %ld for count %ld\n", src->allocated,
    // count);

    hipMalloc((void **)&srcBuff, src->allocated);
    hipMalloc((void **)&destBuff, sizeof(gka_decimal_t) * count);
    hipMemcpy(srcBuff, src->data, src->allocated, hipMemcpyHostToDevice);

    hipLaunchKernelGGL(
        gkaHipProcessBlock, dim3((count / blockSize) + 1), dim3(blockSize), 0,
        0, destBuff, srcBuff, elapsed, rate
    );

    // printf("about to call the device\n");
    hipMemcpy(
        dest, destBuff, sizeof(gka_decimal_t) * count, hipMemcpyDeviceToHost
    );

    hipFree(srcBuff);
    hipFree(destBuff);

    for (int i = 0; i < 10 /*count*/; i++) {
      printf("%lf\n", dest[i]);
    }
  };
};