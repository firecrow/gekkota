#ifndef __GKA_AUDIO_SEGMENT_HEADER
#define __GKA_AUDIO_SEGMENT_HEADER

#include <math.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if GKA_GPU_ARCH == hip
#include <hip/hip_runtime.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#if GKA_GPU_ARCH == hip
#define __PROCESS_GPU__ __device__
#define __PROCESS_HOST__
#define __PROCESS_BRIDGE__ __global__
#define __PROCESS_BOTH__ __host__ __device__
#else
#define __PROCESS_GPU__
#define __PROCESS_HOST__
#define __PROCESS_BRIDGE__
#define __PROCESS_BOTH__
#endif

#define MAX_PHASE 2. * M_PI

/* boundry action is not necessarliy bad, it happens at the end of a block and at
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

/**
 * These are the entry types, indicating what type of entry it is. This is used
 * for both calculation and traversal on the CPU and GPU
*/
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

/** 
 * These are transition types for softening the changes in value, any of these
 * can be chosen when processing a difference in value between two segment
 * entries */
enum gka_transition_type {
  GKA_CLIFF = 0,
  GKA_LINEAR,
  GKA_EASE_IN,
  GKA_EASE_OUT,
};

/* These structs are packed into a block of memory that is sent to the GPU or
 * CPU Host process to run linear algebra calculations based on the patterns
 * that these structs describe. it is an array (loosely) where records can
 * indicate a place to continue on from in the event of segmentation
 * */
struct gka_entry {
  union {
    /* This struct includes all variables that are common to all structs */
    struct {
      /* this is the set of values which determing if this is a sound, a
       * segment, or an indication to continue at another address this is the
       * indication of what type of entry it is.
       */
      gka_operand_t type;
    } all;
    /* This is the first struct in the block, containing information about how
     * large the block is and where the first sound is located, and some
     * tracking for where to insert new structs into this block*/
    struct {
      gka_operand_t type;
      gka_local_address_t allocated;
      gka_local_address_t next_available;
      /* this is the address of the first segment if it is a sound, or the next
       * segment if this indidates to continue on at another address */
      gka_local_address_t addr;
    } head;
    /* This indicates that the struct that is being sought after at this address
     * is located, and it's array continues, somewhere else*/
    struct {
      gka_operand_t type;
      /* this is the address of the first segment if it is a sound, or the next
       * segment if this indidates to continue on at another address */
      gka_local_address_t addr;
    } link;
    /* Segments are the building blocks of sound data, such as frequency,
     * volume, or shape values, this is the base unit of information used for
     * calculations*/
    struct {
      gka_operand_t type;
      /* this is the function identifier of the function used to smooth
        transitions such as ease, it uses values fro gka_transition_type enum */
      gka_operand_t transition;
      gka_decimal_t value;
      gka_time_t start_time;
    } segment;
    /* An event represents an `instance` of a sound, so this contains
     * information about when it starts and how often it repeats*/
    struct {
      gka_operand_t type;
      gka_local_address_t sounds;
      gka_time_t start;
      gka_time_t repeat;
    } event;
    /* The sound struct is a holding place for the segments that make up the
     * sounds behaviour, phase is stored here in order to calculate a smooth
     * position between periods*/
    struct {
      gka_operand_t type;
      gka_decimal_t phase;
      gka_local_address_t freq;
      gka_local_address_t volume;
    } sound;
  } values;
};

/***************************************************************************
segment and memory management

This section is about allocating and updating the gka_entry struct(s) into a
block of memory
***************************************************************************/

/**
 * Create a block of memory of the size `size` enter information about it such
 * as what is allocated and where the next available address is in the first
 * slot of the block
 */
__PROCESS_BOTH__ struct gka_entry *gka_alloc_memblock(gka_local_address_t size);

/** 
 * Returns the next available space and increments the available space address
 * pointer
 */
gka_local_address_t __PROCESS_BOTH__
gka_allocate_space(struct gka_entry *blk, gka_local_address_t size);

/**
 * Converts a global address (64bit) into a local address (32bit) which starts
 * at the start of the block it is contained in
*/
__PROCESS_BOTH__ gka_local_address_t
gka_to_local(struct gka_entry *blk, struct gka_entry *entry);

/**
 * Converts a local address (32bit offset from the start of it's block) to a
 * global address, to be usable in referencing the structs memory 
*/
__PROCESS_BOTH__ struct gka_entry *
gka_pointer(struct gka_entry *blk, gka_local_address_t localp);

/**
 * Retrieves global address of the nth entry in a block
*/
__PROCESS_BOTH__ struct gka_entry *gka_nth(struct gka_entry *blk, int offset);

/**
 * Increment the space allocated past the requested entry, there by claiming it
 * as not available
*/
__PROCESS_BOTH__ int
gka_claim_entry(struct gka_entry *blk, gka_local_address_t localp);

/**
 * Retrieve the next (sibling) global address from the local address supplied
*/
__PROCESS_BOTH__ struct gka_entry *
gka_next(struct gka_entry *blk, gka_local_address_t localp);

/** 
 * Retrieve the next (sibling) local address from the local address supplied
*/
__PROCESS_BOTH__ gka_local_address_t
gka_next_local(struct gka_entry *blk, gka_local_address_t localp);

/**
 * Add an entry to the array of the existing item suuplied as `localp` if the
 * next space is taken, creates a link entry to indicate where to continue
 * 
 * This also reserves the next cell after that as the place for the next record,
 * or a link record if necessary
 * 
 * returns the new local address
*/
__PROCESS_BOTH__ gka_local_address_t
gka_extend_entry(struct gka_entry *blk, gka_local_address_t localp);

/**
 * Add an entry to the end of the set of thie supplied `type` of entry 
*/
__PROCESS_BOTH__ gka_local_address_t gka_add_entry_to_set(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
);

/**
 * Fancy wrapper for setting the `type` property of an entry
*/
__PROCESS_BOTH__ void gka_set_entry_status(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
);

/**
 * Traverse the block and retrieve the next entry in the set
*/
__PROCESS_BOTH__ gka_local_address_t gka_entry_next(
    struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type
);

/**
 * Creates a `segment` type entry, allocated and residing on the block
*/
__PROCESS_BOTH__ gka_local_address_t gka_segment_create(
    struct gka_entry *blk, gka_value_t gotime, gka_decimal_t start_value,
    gka_operand_t ease
);

/**
 * Climb through ad add segment to the end of the set
*/
__PROCESS_BOTH__ gka_local_address_t gka_segpattern_add_segment(
    struct gka_entry *blk, gka_local_address_t current, struct gka_entry *seg
);

/**
 * Creates a segment and reservers the neighbor next to it
*/
__PROCESS_BOTH__ gka_local_address_t gka_segment_new(
    struct gka_entry *blk, gka_value_t start_time, gka_decimal_t start_value,
    gka_operand_t ease
);

/**
 * Creates a segment with the given values on the block
*/
__PROCESS_BOTH__ gka_local_address_t gka_segpattern_add_segment_values(
    struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t start_time,
    gka_decimal_t value, gka_operand_t transition
);

/**
 * Creates a segment with the values of the given supplied struct on the block
*/
__PROCESS_BOTH__ gka_local_address_t gka_extend_segment(
    struct gka_entry *blk, gka_local_address_t current, struct gka_entry *seg
);

/**
 * Creatse an entry of type `sound` on the block
*/
__PROCESS_BOTH__ gka_local_address_t gka_sound_create(
    struct gka_entry *blk, gka_local_address_t freq, gka_local_address_t volume
);

/**
 * Creates an entry of the type `event` on the block
*/
__PROCESS_BOTH__ gka_local_address_t gka_sound_event_create(
    struct gka_entry *blk, gka_local_address_t sounds, gka_time_t start,
    gka_time_t repeat
);

/* ------ retrieve and process segment values ------ */

/**
 * This function counds the number of sounds contained in a given block, this is
 * important becuase memory must be allocated seperately on the GPU to calculate
 * the `step` and `phase` information during rendering
*/
int gka_count_sounds_in_block(struct gka_entry *blk);

/**
 * This traverses the block starting with the current local address of a segment
 * to find the relevant segment based on the `offset` time, which is the real
 * world value of time for the sound being played
*/
__PROCESS_BOTH__ struct gka_entry *gka_segment_from_pattern(
    struct gka_entry *blk, gka_local_address_t currentlp, gka_time_t offset
);

/**
 * Retrieves the value of an `offset` time in the context of a given set of
 * segments
*/
__PROCESS_BOTH__ double value_from_segment(
    struct gka_entry *blk, gka_local_address_t current,
    gka_decimal_t base_value, gka_time_t offset
);

/**
 * The modulus of a time value, used for repeating sounds which need to be
 * "re-based" onto a time series other than global time
*/
__PROCESS_BOTH__ gka_time_t gka_time_modulus(gka_time_t src, gka_time_t mod);

/*******************************************************************************
math and transition functions

These functions are used to "soften" transitions between values
********************************************************************************/
__PROCESS_BOTH__ void square_ease(
    double *progress, struct gka_entry *segment, double start, double end
);
__PROCESS_BOTH__ void linear_ease(
    double *progress, struct gka_entry *segment, double start, double end
);
__PROCESS_BOTH__ void ease_flip(
    double *progress, struct gka_entry *segment, double start, double end
);
__PROCESS_BOTH__ void
ease_in(double *progress, struct gka_entry *segment, double start, double end);
__PROCESS_BOTH__ void
ease_out(double *progress, struct gka_entry *segment, double start, double end);
__PROCESS_BOTH__ void ease_inout(
    double *progress, struct gka_entry *segment, double start, double end
);

/**
 * This is the function that provides the mapping between the easing functions
 * and the value indicating which function to apply in the segment entries
*/
__PROCESS_BOTH__ void gka_apply_transition(
    double *progress, gka_operand_t transition, struct gka_entry *segment,
    double start, double end
);

#endif