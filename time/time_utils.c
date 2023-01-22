#include <time.h>
#include "../block-sound-mem/block-sound-mem.h"
#include "gka_time.h"

gka_time_t from_timespec(struct timespec *t) {
  return t->tv_sec * NANOS + t->tv_nsec;
}

/** returns -1 if it is not within on second (save room for signed), the nano
 * second delta otherwise
 *
 * it does clamp to values from 0.0..1.0
 */
double timespec_position(gka_time_t start, gka_time_t end, gka_time_t current) {
  gka_time_t delta = current - start;
  gka_time_t begining_to_end = end - start;

  double value = delta / begining_to_end;

  if (value <= 1.0) {
    return value;
  }
  return 0.0;
}

gka_time_t gka_now() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec * NANOS + t.tv_nsec;
}

gka_time_t gka_time_modulus(gka_time_t src, gka_time_t mod) {
  gka_time_t remainder = src;
  if (src > mod) {
    gka_time_t times = src / mod;
    remainder = src - mod * times;
  }
  return remainder;
}