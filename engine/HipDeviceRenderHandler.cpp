#include "gekkota.h"
#include "audio-segment/hip-calculations.h"

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
    double *stepsBuff;
    double *phasesBuff;

    // test_print_mem_block(src);

    int soundCount = gka_count_sounds_in_block(src);

    hipMalloc((void **)&stepsBuff, soundCount * count * sizeof(double));
    hipMalloc((void **)&phasesBuff, soundCount * count * sizeof(double));

    double *debugSteps = (double *)malloc(soundCount * count * sizeof(double));
    double *debugPhases = (double *)malloc(soundCount * count * sizeof(double));

    hipMalloc((void **)&srcBuff, src->values.head.allocated);
    hipMalloc((void **)&destBuff, sizeof(gka_decimal_t) * count);
    hipMemcpy(srcBuff, src, src->values.head.allocated, hipMemcpyHostToDevice);

    // calculate steps in parallel
    hipLaunchKernelGGL(
        gkaHipSetSteps, dim3((count / blockSize) + 1), dim3(blockSize), 0, 0,
        stepsBuff, srcBuff, elapsed, rate, count
    );

    // calculate phase in series
    hipLaunchKernelGGL(
        gkaHipSetPhases, dim3((soundCount / blockSize) + 1), dim3(blockSize), 0,
        0, phasesBuff, stepsBuff, count, soundCount
    );

    // process sound
    hipLaunchKernelGGL(
        gkaHipProcessBlock, dim3((count / blockSize) + 1), dim3(blockSize), 0,
        0, destBuff, srcBuff, phasesBuff, elapsed, rate, count
    );

    hipMemcpy(
        debugSteps, stepsBuff, sizeof(double) * count * soundCount,
        hipMemcpyDeviceToHost
    );

    hipMemcpy(
        debugPhases, phasesBuff, sizeof(double) * count * soundCount,
        hipMemcpyDeviceToHost
    );

    hipMemcpy(
        dest, destBuff, sizeof(gka_decimal_t) * count, hipMemcpyDeviceToHost
    );

    hipFree(stepsBuff);
    hipFree(phasesBuff);
    hipFree(srcBuff);
    hipFree(destBuff);

    // free(debugSteps);
    free(debugPhases);

    /*
    printf("steps...\n");
    for (int i = 0; i < count; i++) {
      printf("%lf\n", debugSteps[i]);
    }
    */
    printf("phases...\n");
    for (int i = 0; i < count; i++) {
      printf("%lf\n", debugPhases[i]);
    }
    /*
    printf("audio data...\n");
    for (int i = 0; i < count; i++) {
      printf("%lf\n", dest[i]);
    }
    */

    // debug
    // exit(1);
  };
};