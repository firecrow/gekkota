#include "../gekkota.h"

static const char *device = "hw:4,0"; /* playback device */

static double generate_frame_value(
    vector<struct gka_entry *> blocks, gka_time_t local, const uint32_t rate
) {
  double frame_value = 0.0;
  int sound_id = 0;

  for (struct gka_entry *m : blocks) {
    frame_value += gka_frame_from_block(m, local, rate);
  }

  fprintf(Title::instance.logger.file, "%lf\n", frame_value);
  return frame_value;
}

static void generate_sine(
    const struct gka_audio_params &gka_params,
    const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset, int count,
    const double *data
) {

  unsigned char *samples[gka_params.channels];
  int steps[gka_params.channels];
  unsigned int chn;

  int format_bits = snd_pcm_format_width(gka_params.format);

  unsigned int maxval = (1 << (format_bits - 1)) - 1;
  int bps = format_bits / 8; /* bytes per sample */
  int phys_bps = snd_pcm_format_physical_width(gka_params.format) / 8;

  /* verify and prepare the contents of areas */
  for (chn = 0; chn < gka_params.channels; chn++) {
    if ((areas[chn].first % 8) != 0) {
      printf("areas[%u].first == %u, aborting...\n", chn, areas[chn].first);
      exit(EXIT_FAILURE);
    }
    samples[chn] = /*(signed short *)*/ (
        ((unsigned char *)areas[chn].addr) + (areas[chn].first / 8)
    );
    steps[chn] = areas[chn].step / 8;
    samples[chn] += offset * steps[chn];
  }

  int res = 0;
  double frame_value = 0;
  gka_time_t local;

  /* fill the channel areas */
  int frame = 0;

  for (int f = 0; f < count; f++) {
    res = data[f] * maxval;
    int i;
    for (chn = 0; chn < gka_params.channels; chn++) {
      for (i = 0; i < bps; i++)
        *(samples[chn] + i) = (res >> i * 8) & 0xff;
      samples[chn] += steps[chn];
    }
  }
}

static int xrun_recovery(snd_pcm_t *handle, int err) {
  if (err == -EPIPE) { /* under-run */
    err = snd_pcm_prepare(handle);
    if (err < 0)
      printf(
          "Can't recovery from underrun, prepare failed: %s\n",
          snd_strerror(err)
      );
    return 0;
  } else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume(handle)) == -EAGAIN)
      sleep(1); /* wait until the suspend flag is released */
    if (err < 0) {
      err = snd_pcm_prepare(handle);
      if (err < 0)
        printf(
            "Can't recovery from suspend, prepare failed: %s\n",
            snd_strerror(err)
        );
    }
    return 0;
  }
  return err;
}

struct gka_output_objects *
allocate_framegroup_memory(const struct gka_audio_params &gka_params) {
  short *samples = (short *)malloc(
      (period_size * gka_params.channels *
       snd_pcm_format_physical_width(gka_params.format)) /
      8
  );

  snd_pcm_channel_area_t *areas = (snd_pcm_channel_area_t *)calloc(
      gka_params.channels, sizeof(snd_pcm_channel_area_t)
  );
  if (areas == NULL) {
    printf("No enough memory\n");
    exit(EXIT_FAILURE);
  }
  int chn;
  for (chn = 0; chn < gka_params.channels; chn++) {
    areas[chn].addr = samples;
    areas[chn].first = chn * snd_pcm_format_physical_width(gka_params.format);
    areas[chn].step =
        gka_params.channels * snd_pcm_format_physical_width(gka_params.format);
  }

  gka_output_objects *output_objects = new gka_output_objects();
  output_objects->areas = areas;
  output_objects->samples = samples;
  return output_objects;
}

int write_loop(const struct gka_audio_params &gka_params) {
  cout << "samle rate from write_loop: " << gka_params.rate << endl;

  gka_output_objects *output_objects = allocate_framegroup_memory(gka_params);

  signed short *ptr;
  int r, cptr;

  while (AudioOutputService::running) {

    Engine *engine = &Engine::instance;
    Title *ctx = &Title::instance;
    const double *data = engine->render(
        ctx->sound_blocks, gka_params.period_size, gka_params.rate
    );

    // const double *data = generate_data(period_size, gka_params.rate);
    generate_sine(
        gka_params, output_objects->areas, 0, gka_params.period_size, data
    );

    ptr = output_objects->samples;
    cptr = period_size;
    while (cptr > 0) {
      r = snd_pcm_writei(gka_params.output_handle, ptr, cptr);
      if (r == -EAGAIN)
        continue;
      if (r < 0) {
        if (xrun_recovery(gka_params.output_handle, r) < 0) {
          printf("Write ror: %s\n", snd_strerror(r));
          exit(EXIT_FAILURE);
        }
        break;
      }
      ptr += r * gka_params.channels;
      cptr -= r;
    }
  }

  printf("in bottom of write loop function\n");
  free(output_objects->areas);
  free(output_objects->samples);
  free(output_objects);
  return 0;
}

int setup_hw(struct gka_audio_params *gka_params) {
  printf("Setting up hw in engine\n");

  int err;
  unsigned int chn;
  snd_pcm_channel_area_t *areas;
  signed short *samples;

  printf("Playback device is %s\n", device);
  printf(
      "Stream parameters are %uHz, %s, %u channels\n", gka_params->rate,
      snd_pcm_format_name(gka_params->format), gka_params->channels
  );

  snd_pcm_t *handle;
  GkaSetAlsaHw(snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0));
  gka_params->output_handle = handle;

  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  if ((err = set_hwparams(
           gka_params, gka_params->output_handle, hwparams,
           SND_PCM_ACCESS_RW_INTERLEAVED
       )) < 0) {
    printf("Setting of hwparams failed: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }
  buffer_size = gka_params->buffer_size;
  period_size = gka_params->period_size;

  if ((err = set_swparams(gka_params, gka_params->output_handle, swparams)) <
      0) {
    printf("Setting of swparams failed: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }

  printf("Setup complete\n");
  return 0;
}

void tear_down_audio(struct gka_audio_params *gka_params) {
  printf("tearing down handle %ld\n", gka_params->output_handle);
  snd_pcm_close(gka_params->output_handle);
}
