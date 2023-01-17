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
  m->allocated = size;

  // reserve first slot
  m->next_available += sizeof(struct gka_entry);

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
    return ((gka_global_t)blk->data) - (gka_global_t)entry;
}

struct gka_entry *gka_pointer(struct gka_mem_block *blk, gka_local_address_t localp){
    if(localp < 0 || localp > blk->allocated ){
        return GKA_BOUNDRY_ACTION;
    }
    return (void *)(blk->data + localp);
}

struct gka_entry *gka_nth(struct gka_mem_block *blk, int offset){
    return gka_pointer(blk, sizeof(struct gka_entry)*offset);
}

gka_local_address_t gka_next_local(struct gka_mem_block *blk, gka_local_address_t localp){
    gka_local_address_t next_would_be = localp + sizeof(struct gka_entry);
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
    gka_local_address_t next_would_be = localp + sizeof(struct gka_entry);
    printf("claim.... %ld/%ld %ld\n", next_would_be, blk->allocated, blk->allocated/sizeof(struct gka_entry));
    if(next_would_be > blk->allocated){
        return GKA_BOUNDRY_ACTION; 
    } 
    if(blk->next_available < next_would_be){ 
        blk->next_available = next_would_be;
    }
    return next_would_be;
}