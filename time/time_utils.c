#include <time.h>
#include "gka_time.h"

gka_time_t from_timespec(struct timespec *t) {
  return t->tv_sec * NANOS + t->tv_nsec;
}

gka_time_t gka_now() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec * NANOS + t.tv_nsec;
}