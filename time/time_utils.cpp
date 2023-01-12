#include "../gekkota.h"

gka_timeint from_timespec(struct timespec *t) {
  return t->tv_sec * NANOS + t->tv_nsec;
}

/** returns -1 if it is not within on second (save room for signed), the nano
 * second delta otherwise
 *
 * it does clamp to values from 0.0..1.0
 */
double
timespec_position(gka_timeint start, gka_timeint end, gka_timeint current) {
  gka_timeint delta = current - start;
  gka_timeint begining_to_end = end - start;

  double value = delta / begining_to_end;

  if (value <= 1.0) {
    return value;
  }
  return 0.0;
}

gka_timeint gka_now() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec * NANOS + t.tv_nsec;
}

gka_timeint gka_time_modulus(gka_timeint src, gka_timeint mod) {
  gka_timeint remainder = src;
  if (src > mod) {
    gka_timeint times = src / mod;
    remainder = src - mod * times;
  }
  return remainder;
}