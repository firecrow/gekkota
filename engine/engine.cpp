#include "../gekkota.h"

static const char *device = "hw:3,0"; /* playback device */

static double generate_frame_value(
    Title &ctx, vector<SoundEvent *> events, gka_timeint local,
    const gka_audio_params &gka_params
) {
  double frame_value = 0.0;
  int sound_id = 0;
  for (SoundEvent *s : events) {
    // fprintf(Title::instance.logger.file, "rendering sound %d\n", sound_id++);
    if (s->repeat != nullptr) {
      gka_timeint local_repeat;
      if (s->repeat->start > local) {
        continue;
      }
      if (s->repeat->time == 0) {
        continue;
      }
      local_repeat =
          gka_time_modulus(local - s->repeat->start, s->repeat->time);
      frame_value +=
          s->getFrameValue(s->repeat->start, local_repeat, gka_params);
      // fprintf(
      //     ctx.logger.file, "local/local repeat: %ld/%ld\n", local,
      //     local_repeat
      //);
    } else {
      // fprintf(Title::instance.logger.file, "from local %ld\n", local);
      frame_value += s->getFrameValue(s->gotime, local, gka_params);
    }
  }
  fprintf(Title::instance.logger.file, "%lf\n", frame_value);
  return frame_value;
}

static void generate_sine(
    const struct gka_audio_params &gka_params,
    const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset, int count
) {
  double volume = 0.0;
  gka_timeint elapsed;

  Title *ctx = &Title::instance;

  static double max_phase = 2. * M_PI;

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
    if ((areas[chn].step % 16) != 0) {
      printf("areas[%u].step == %u, aborting...\n", chn, areas[chn].step);
      exit(EXIT_FAILURE);
    }
    steps[chn] = areas[chn].step / 8;
    samples[chn] += offset * steps[chn];
  }

  int res = 0;
  double frame_value = 0;
  gka_timeint local;

  /* fill the channel areas */
  int frame = 0;
  elapsed = gka_now() - ctx->start_time;

  while (count-- > 0) {
    frame++;
    res = 0;
    frame_value = 0.0;
    local = elapsed + frame;

    frame_value =
        generate_frame_value(*ctx, ctx->sound_events, local, gka_params);

    res = frame_value * maxval;
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

  double volume = 0.0;
  signed short *ptr;
  int r, cptr;

  while (AudioOutputService::running) {
    generate_sine(gka_params, output_objects->areas, 0, period_size);

    // fprintf(Title::instance.logger.file, "sine generated\n");

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
  snd_pcm_close(gka_params->output_handle);
}
