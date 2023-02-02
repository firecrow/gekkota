#include <inttypes.h>

/**
 * macro definitions for common times used throughout the music composition side
 * of the application
 */
#define NANOS 1000000000
#define BEAT_NS 500000000
#define BEAT_NS_32nds 15625000
#define FRAME_NANOS 10417

/**
 * unit used to track time throughout the application
 */
typedef uint64_t gka_time_t;

/**
 * This converts a timespec into a single 64 bit double, which retains enough
 * room for the demands of music composition which does not include the full
 * scale of what a timespec could hold
 */
gka_time_t from_timespec(struct timespec *t);

/**
 * current time in nanoseconds on the host system
 */
gka_time_t gka_now();