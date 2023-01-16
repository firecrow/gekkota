#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#define GKA_BOUNDRY_ACTION 0
#define GKA_SUCCESS 0
#define GKA_MEMORY_FAILURE 7

typedef uint64_t gka_global_t;
typedef uint64_t gka_value_t;
typedef double gka_decimal_t;
typedef uint32_t gka_subvalue_t;
typedef char gka_operand_t;
typedef gka_subvalue_t gka_local_address_t;
typedef gka_value_t gka_time_t;

struct gka_mem_block {
  gka_local_address_t next_available;
  gka_subvalue_t allocated;
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

/* This is the basic block layout that other objects will fit itno*/
struct gka_entry {
  /* this is the set of values which determing if this is a sound, a segment, or
   * an indication to continue at another address 
   * this is the indication of what type of entry it is, uses the gka_value_operand enum*/
  gka_operand_t type;
  gka_operand_t subtype;
  /* this is the function identifier of the function used to smooth
    transitions such as ease, it uses values fro gka_transition_type enum */
  gka_operand_t transition;
  /* this is the address of the first segmetn if it is a sound, or the next
    * segment if this indidates to continue on at another address */
  gka_local_address_t addr;
  union {
    /* this struct is used if it is a sound refering to child segments*/
    struct {
      gka_time_t start;
      gka_time_t repeat;
    } schedule;
    /* this struct is used if it is a segment*/
    struct {
      gka_value_t value;
      gka_time_t start_time;
    } placement;
  } values;
};

struct gka_mem_block *gka_alloc_memblock(gka_local_address_t size);
gka_local_address_t gka_to_local(struct gka_mem_block *blk, struct gka_entry *entry);
gka_local_address_t gka_allocate_space(struct gka_mem_block *blk, gka_local_address_t size);
struct gka_entry *gka_pointer(struct gka_mem_block *blk, gka_local_address_t localp);