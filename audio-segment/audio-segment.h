#ifndef __GKA_AUDIO_SEGMENT_HEADER
#define __GKA_AUDIO_SEGMENT_HEADER

#include <math.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_PHASE 2. * M_PI

#include "time/gka_time.h"

/* boundry action is not necessarliy bad it happens at the end of a block and at
 * the end of a segment, both of which can happen naturally */
#define GKA_BOUNDRY_ACTION 0
#define GKA_SUCCESS 0
#define GKA_MEMORY_FAILURE 7

#define GKA_SEGMENT_SIZE sizeof(struct gka_entry)

// 8
typedef char gka_operand_t;
// 32
typedef uint32_t gka_subvalue_t;
typedef gka_subvalue_t gka_local_address_t;
typedef float gka_subdecimal_t;
// 64
typedef uint64_t gka_global_t;
typedef uint64_t gka_value_t;
typedef double gka_decimal_t;
typedef gka_value_t gka_time_t;

enum gka_value_operand {
  GKA_UNSPECIFIED = 0,
  GKA_HEAD,
  GKA_RESERVED_BY_NEIGHBOUR,
  GKA_SOUND,
  GKA_SOUND_EVENT,
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

/* This is the basic block layout that other objects will fit into*/
struct gka_entry {
  union {
    struct {
      /* this is the set of values which determing if this is a sound, a
       * segment, or an indication to continue at another address this is the
       * indication of what type of entry it is, uses the gka_value_operand enum
       */
      gka_operand_t type;
      gka_operand_t subtype;
    } all;
    struct {
      gka_operand_t type;
      gka_local_address_t allocated;
      gka_local_address_t next_available;
      /* this is the address of the first segment if it is a sound, or the next
       * segment if this indidates to continue on at another address */
      gka_local_address_t addr;
    } head;
    struct {
      gka_operand_t type;
      /* this is the address of the first segment if it is a sound, or the next
       * segment if this indidates to continue on at another address */
      gka_local_address_t addr;
    } link;
    struct {
      gka_operand_t type;
      /* this is the function identifier of the function used to smooth
        transitions such as ease, it uses values fro gka_transition_type enum */
      gka_operand_t transition;
      gka_decimal_t value;
      gka_time_t start_time;
    } segment;
    struct {
      gka_operand_t type;
      gka_local_address_t sounds;
      gka_time_t start;
      gka_time_t repeat;
    } event;
    struct {
      gka_operand_t type;
      gka_decimal_t phase;
      gka_local_address_t freq;
      gka_local_address_t volume;
    } sound;
  } values;
};

/* ------ segment and memory management ------ */
struct gka_entry *gka_alloc_memblock(gka_local_address_t size);

gka_local_address_t
gka_allocate_space(struct gka_entry *blk, gka_local_address_t size);

gka_local_address_t
gka_to_local(struct gka_entry *blk, struct gka_entry *entry);

struct gka_entry *
gka_pointer(struct gka_entry *blk, gka_local_address_t localp);

struct gka_entry *gka_nth(struct gka_entry *blk, int offset);
int gka_claim_entry(struct gka_entry *blk, gka_local_address_t localp);

struct gka_entry *gka_next(struct gka_entry *blk, gka_local_address_t localp);

gka_local_address_t
gka_next_local(struct gka_entry *blk, gka_local_address_t localp);

gka_local_address_t
gka_extend_entry(struct gka_entry *blk, gka_local_address_t localp);

gka_local_address_t gka_add_entry_to_set(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
);

void gka_set_entry_status(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
);

gka_local_address_t gka_entry_next(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
);

gka_local_address_t gka_segment_create(
    struct gka_entry *blk, gka_value_t gotime, gka_decimal_t start_value,
    gka_operand_t ease
);


gka_local_address_t gka_segpattern_add_segment(
    struct gka_entry *blk, gka_local_address_t current, struct gka_entry *seg
);

gka_local_address_t gka_segpattern_add_segment_values(
    struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t start_time,
    gka_decimal_t value, gka_operand_t transition
);

gka_local_address_t gka_extend_segment(
    struct gka_entry *blk, gka_local_address_t current, struct gka_entry *seg
);

gka_local_address_t gka_sound_create(
    struct gka_entry *blk, gka_local_address_t freq, gka_local_address_t volume
);

gka_local_address_t gka_sound_event_create(
    struct gka_entry *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
);


/* ------ retrieve and process segment values ------ */

int gka_count_sounds_in_block(struct gka_entry *blk);

struct gka_entry *gka_segment_from_pattern(
    struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t offset
);

double value_from_segment(
    struct gka_entry *blk, gka_local_address_t current,
    gka_decimal_t base_value, gka_time_t offset
);

gka_decimal_t gka_get_frame_value_from_event(
    struct gka_entry *blk, struct gka_entry *event, gka_time_t start,
    gka_time_t local, const uint32_t rate
);

gka_decimal_t
gka_frame_from_block(struct gka_entry *blk, gka_time_t local, int rate);

/* ------ math and transition functions ------ */
void square_ease(
    double *progress, struct gka_entry *segment, double start, double end
);
void linear_ease(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_flip(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_in(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_out(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_inout(
    double *progress, struct gka_entry *segment, double start, double end
);

void gka_apply_transition(
    double *progress, gka_operand_t transition, struct gka_entry *segment, double start, double end
);

#endif