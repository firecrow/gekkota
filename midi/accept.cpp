#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <alsa/asoundlib.h>
#include <signal.h>

#include "../gekkota.h"

int midi_accept() {
  int i;
  int err;
  char *device_in = "hw:3,0,0";
  int clock_type = -1;
  unsigned char buf[1024];
  ssize_t r;

  snd_rawmidi_t *handle_in = 0;

  err = snd_rawmidi_open(&handle_in, NULL, device_in, 0);
  if (err) {
    fprintf(stderr, "snd_rawmidi_open %s failed: %d\n", device_in, err);
  }

  while (MidiService::running) {
    r = snd_rawmidi_read(handle_in, buf, sizeof(buf));

    if (r == 3) {
      dispatch_to_instrument((struct gka_midi_event *)buf);
    } else if (r < 0) {
      fprintf(stderr, "read error: %d - %s\n", (int)r, snd_strerror(r));
    } else {
      fprintf(stderr, "recived more than a 3 byte event, not implemented");
    }
  }

  if (handle_in) {
    snd_rawmidi_drain(handle_in);
    snd_rawmidi_close(handle_in);
  }

  return 0;
}
