#include <inttypes.h>

#define GKA_BOUNDRY_ACTION 0
#define GKA_SUCCESS 0
#define GKA_MEMORY_FAILURE 7

typedef uint64_t gka_value;
typedef uint32_t gka_subvalue;
typedef double gka_decimal;
typedef gka_subvalue gka_local_address;
typedef gka_value gka_time_t;

struct gka_mem_block {
  gka_local_address next_available;
  gka_subvalue allocated;
  void *data;
};

enum gka_value_operand {
  GKA_UNSPECIFIED = 0,
  GKA_RESERVED_BY_NEIGHBOUR,
  GKA_SOUND,
  GKA_END,
  GKA_SEGMENT_VALUE,
  GKA_NEXT_LOCAL,
};

enum gka_transition_type {
  GKA_CLIFF = 0,
  GKA_LINEAR,
  GKA_EASE_IN,
  GKA_EASE_OUT,
};

enum gka_sound_flags {
  GKA_REPEATS = 0,
};

struct gka_segment_head {
  char operand;
  char : 0;
  char : 0;
  char : 0;
  union {
    gka_subvalue transition;
    gka_local_address entry_on_next_block;
  } head_op;
};

/* This is the basic block layout that other objects will fit itno*/
struct gka_entry {
  struct gka_segment_head head;
  gka_value dimension;
  gka_value value;
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

gka_local_address gka_allocate_space(struct gka_mem_block *blk, size_t size);
void *gka_pointer(struct gka_mem_block *blk, gka_local_address localp);