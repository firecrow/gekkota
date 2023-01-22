#include <inttypes.h>

#define NANOS 1000000000
#define BEAT_NS 500000000
#define BEAT_NS_32nds 15625000
#define FRAME_NANOS 10417

typedef uint64_t gka_time_t;

double timespec_position(gka_time_t start, gka_time_t end, gka_time_t current);

int subtract_time(gka_time_t a, gka_time_t b, gka_time_t out);

gka_time_t from_timespec(struct timespec *t);

gka_time_t gka_time_modulus(gka_time_t src, gka_time_t mod);

gka_time_t gka_now();