#include <inttypes.h>

typedef uint64_t gka_value;
typedef uint32_t gka_sub_value;
typedef double gka_decimal;
typedef gka_sub_value gka_local_address;
typedef gka_value gka_time_t;

enum gka_value_operand {
  GKA_UNSPECIFIED = 0,
  GKA_SOUND,
  GKA_END,
  GKA_SEGMENT_VALUE,
  GKA_NEXT_BLOCK,
};

enum gka_sound_flags {
  GKA_REPEATS = 0,
};

/* This is the basic block layout that other objects will fit itno*/
struct gka_block {
  gka_value op;
  gka_value dimension;
  gka_value value;
};

struct gka_segment_head {
  char operand;
  char : 0;
  char : 0;
  char : 0;
  gka_sub_value transition;
};

struct gka_segment_reference {
  gka_local_address root;
  gka_local_address current;
};

struct gka_sound {
  struct gka_segment_head head;
  gka_local_address frequency;
  gka_local_address volume;
  gka_local_address shape;
  gka_local_address distortion;
};

struct gka_sound_head {
  char operand;
  char sound_flags;
  char : 0;
  char : 0;
  gka_local_address sound;
};

struct gka_sound_event {
  struct gka_sound_head head;
  gka_time_t start;
  gka_time_t repeat;
};

struct gka_segment {
  struct gka_segment_head head;
  gka_time_t gotime;
  gka_decimal value;
};