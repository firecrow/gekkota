#include "block-sound-mem.h"

struct gka_mem_block *gka_alloc_memblock(gka_local_address_t size){
  struct gka_mem_block *m = (struct gka_mem_block *) malloc(sizeof (struct gka_mem_block));
  if(m == NULL){
    fprintf(stderr, "Error allocating memory %s:%d", __FILE__, __LINE__);
    exit(1);
  }
  m->next_available = 0;
  m->data = malloc(size);
  if(m->data == NULL){
    fprintf(stderr, "Error allocating memory %s:%d", __FILE__, __LINE__);
    exit(1);
  }

  memset(m->data, 0, size);
  m->allocated = size;

  // reserve first slot for first sound event
  m->next_available += GKA_SEGMENT_SIZE;

  return m;
}

gka_local_address_t gka_allocate_space(struct gka_mem_block *blk, gka_local_address_t size){
    if(blk->next_available + size > blk->allocated){
            return GKA_BOUNDRY_ACTION; 
    } 
    gka_local_address_t next = blk->next_available;
    blk->next_available += size;
    return next;
}

gka_local_address_t gka_to_local(struct gka_mem_block *blk, struct gka_entry *entry){
    return (gka_global_t)entry - ((gka_global_t)blk->data);
}

struct gka_entry *gka_pointer(struct gka_mem_block *blk, gka_local_address_t localp){
    if(localp < 0 || localp > blk->allocated ){
        return GKA_BOUNDRY_ACTION;
    }
    return (void *)(blk->data + localp);
}

struct gka_entry *gka_nth(struct gka_mem_block *blk, int offset){
    return gka_pointer(blk, GKA_SEGMENT_SIZE*offset);
}

gka_local_address_t gka_next_local(struct gka_mem_block *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
    if(next_would_be > blk->allocated){
        return GKA_BOUNDRY_ACTION; 
    } 
    return next_would_be;
}

struct gka_entry *gka_next(struct gka_mem_block *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = gka_next_local(blk, localp);
    if(next_would_be == GKA_BOUNDRY_ACTION){
        return GKA_BOUNDRY_ACTION;
    }
    return gka_pointer(blk, next_would_be);
}

int gka_claim_entry(struct gka_mem_block *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = localp + GKA_SEGMENT_SIZE;
    if(next_would_be > blk->allocated){
        return GKA_BOUNDRY_ACTION; 
    } 
    if(blk->next_available < next_would_be){ 
        blk->next_available = next_would_be;
    }
    return next_would_be;
}

gka_local_address_t
gka_extend_entry_set(struct gka_mem_block *blk, gka_local_address_t localp, gka_operand_t type){
    printf("\x1b[36mextending\n\x1b[0m");

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

    printf("\x1b[36mavailable reserving next next and returning newlp\n\x1b[0m");
    return neighbour_address;
  }else{
    if(neighbour->values.all.type == GKA_RESERVED_BY_NEIGHBOUR){
      printf("\x1b[36mavailable reserving next next and returning LINK newlp\n\x1b[0m");

      gka_local_address_t newlp =
        gka_allocate_space(blk, GKA_SEGMENT_SIZE);
      neighbour->values.all.type = GKA_NEXT_LOCAL;
      neighbour->values.link.addr = newlp;

      return newlp;
    }
  }
  return 0;
}