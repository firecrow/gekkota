#define NANOS 1000000000
#define BEAT_NS 500000000
#define BEAT_NS_32nds 15625000
#define FRAME_NANOS 10417

typedef uint64_t gka_timeint;

double
timespec_position(gka_timeint start, gka_timeint end, gka_timeint current);

int subtract_time(gka_timeint a, gka_timeint b, gka_timeint out);

gka_timeint from_timespec(struct timespec *t);

gka_timeint gka_time_modulus(gka_timeint src, gka_timeint mod);

gka_timeint gka_now();