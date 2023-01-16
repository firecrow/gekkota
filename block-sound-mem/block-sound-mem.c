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
    printf("space+\n");
    if(blk->next_available + size > blk->allocated){
            return GKA_BOUNDRY_ACTION; 
    } 
    gka_local_address_t next = blk->next_available;
    blk->next_available += size;
    printf("alloc returning %ld\n", next);
    return next;
}

gka_local_address_t gka_to_local(struct gka_mem_block *blk, struct gka_entry *entry){
    return ((gka_global_t)blk->data) - (gka_global_t)entry;
}

struct gka_entry *gka_pointer(struct gka_mem_block *blk, gka_local_address_t localp){
    printf("blk:%ld localp:%ld\n", blk, localp);
    if(localp < 0 || localp > blk->allocated ){
        return GKA_BOUNDRY_ACTION;
    }
    return (void *)(blk->data + localp);
}

struct gka_entry *gka_nth(struct gka_mem_block *blk, int offset){
    return gka_pointer(blk, sizeof(struct gka_entry)*offset);
}