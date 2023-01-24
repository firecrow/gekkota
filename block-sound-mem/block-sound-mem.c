#include "block-sound-mem.h"

struct gka_entry *gka_alloc_memblock(gka_local_address_t size){
  struct gka_entry *m = (struct gka_entry *) malloc(size);
  if(m == NULL){
    fprintf(stderr, "Error allocating memory %s:%d", __FILE__, __LINE__);
    exit(1);
  }
  memset(m, 0, size);

  m->values.head.next_available = GKA_SEGMENT_SIZE;
  m->values.head.allocated = size;
  m->values.all.type = GKA_HEAD;

  return m;
}

gka_local_address_t gka_allocate_space(struct gka_entry *blk, gka_local_address_t size){
    if(blk->values.head.next_available + size > blk->values.head.allocated){
            return GKA_BOUNDRY_ACTION; 
    } 
    gka_local_address_t next = blk->values.head.next_available;
    blk->values.head.next_available += size;
    return next;
}

gka_local_address_t gka_to_local(struct gka_entry *blk, struct gka_entry *entry){
    return (gka_global_t)entry - ((gka_global_t)blk);
}

struct gka_entry *gka_pointer(struct gka_entry *blk, gka_local_address_t localp){
    if(localp < 0 || localp > blk->values.head.allocated ){
        return GKA_BOUNDRY_ACTION;
    }
    return (void *)((gka_global_t)blk + localp);
}

struct gka_entry *gka_nth(struct gka_entry *blk, int offset){
    return gka_pointer(blk, GKA_SEGMENT_SIZE*offset);
}

gka_local_address_t gka_next_local(struct gka_entry *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
    if(next_would_be > blk->values.head.allocated){
        return GKA_BOUNDRY_ACTION; 
    } 
    return next_would_be;
}

struct gka_entry *gka_next(struct gka_entry *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = gka_next_local(blk, localp);
    if(next_would_be == GKA_BOUNDRY_ACTION){
        return GKA_BOUNDRY_ACTION;
    }
    return gka_pointer(blk, next_would_be);
}

int gka_claim_entry(struct gka_entry *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
    if(next_would_be > blk->values.head.allocated){
        return GKA_BOUNDRY_ACTION; 
    } 
    if(blk->values.head.next_available < next_would_be){ 
        blk->values.head.next_available = next_would_be;
    }
    return next_would_be;
}

gka_local_address_t
gka_add_entry_to_set(struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type){
  gka_local_address_t next = localp;
  gka_local_address_t last = next;
  while(next){
    last = next;
    next = gka_entry_next(blk, localp, type);
  }
  printf("rocking on from last:%ld\n", last/GKA_SEGMENT_SIZE);
  return gka_extend_entry(blk, last);
}

gka_local_address_t
gka_extend_entry(struct gka_entry *blk, gka_local_address_t localp){

  gka_local_address_t neighbour_address = gka_next_local(blk, localp);

  if(neighbour_address == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "BLOCK BOUNDS REACHED %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *neighbour = gka_pointer(blk, neighbour_address);

  gka_local_address_t next_neighbour_address = gka_next_local(blk, neighbour_address);
  if(next_neighbour_address == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "BLOCK BOUNDS REACHED %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *next_neighbour = gka_pointer(blk, next_neighbour_address);

  if(neighbour->values.all.type != GKA_UNSPECIFIED && neighbour->values.all.type != GKA_RESERVED_BY_NEIGHBOUR){
    fprintf(stderr, "FATAL MEMORY CLOBBER %s:%d\n", __FILE__, __LINE__);
    exit(1);
    return 0;
  }

  if(next_neighbour_address == GKA_BOUNDRY_ACTION || next_neighbour->values.all.type == GKA_UNSPECIFIED){
    if(!gka_claim_entry(blk, next_neighbour_address)){
      fprintf(stderr, "FATAL MEMORY CLOBBER %s:%d\n", __FILE__, __LINE__);
      exit(1);
      return 0;
    }

    gka_set_entry_status(blk, next_neighbour_address, GKA_RESERVED_BY_NEIGHBOUR);
    next_neighbour->values.all.type = GKA_RESERVED_BY_NEIGHBOUR;

    return neighbour_address;
  }else if(neighbour->values.all.type == GKA_RESERVED_BY_NEIGHBOUR){
    gka_local_address_t newlp =
    gka_allocate_space(blk, GKA_SEGMENT_SIZE);
    neighbour->values.all.type = GKA_NEXT_LOCAL;
    neighbour->values.link.addr = newlp;

    return newlp;
  }else{
    fprintf(stderr, "BLOCK BOUNDS REACHED %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  return 0;
}

gka_local_address_t gka_entry_next(struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type){
  if(0){
    printf("finding next from localp: %ld\n", localp/GKA_SEGMENT_SIZE);
  }

  gka_local_address_t newlp = gka_next_local(blk, localp);
  if(newlp == GKA_BOUNDRY_ACTION){
    fprintf(stderr, "Error getting next address %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  struct gka_entry *s = gka_pointer(blk, newlp);
  if(s->values.all.type == GKA_NEXT_LOCAL){
    return s->values.link.addr;
  }else if(s->values.all.type == type){
    if(0){
      printf("found newlp: %ld\n", newlp/ GKA_SEGMENT_SIZE);
    }
    return newlp;
  }
  return GKA_BOUNDRY_ACTION;
}

void gka_set_entry_status(struct gka_entry *blk, gka_local_address_t localp, gka_operand_t type) {
  struct gka_entry *s = gka_pointer(blk, localp);
  s->values.all.type = type;
}

int gka_count_sounds_in_block(struct gka_entry *blk) {

  int count = 0;
  struct gka_entry *head = gka_pointer(blk, 0);
  gka_local_address_t soundlp = head->values.head.addr;

  while (soundlp) {
    struct gka_entry *e = gka_pointer(blk, soundlp);
    count++;
    soundlp = gka_entry_next(blk, soundlp, GKA_SOUND_EVENT);
  }
  return count;
}