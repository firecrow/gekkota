#include "block-sound-mem.h"

gka_local_address_t gka_allocate_space(struct gka_mem_block *blk, gka_local_address_t size){
   if(blk->allocated + size > blk->allocated){
        return GKA_BOUNDRY_ACTION; 
   } 
   gka_local_address_t next = blk->next_available;
   blk->next_available += size;
   return next;
}

void *gka_pointer(struct gka_mem_block *blk, gka_local_address_t localp){
    if(localp < 0 || localp > blk->allocated ){
        return GKA_BOUNDRY_ACTION;
    }
    return (void *)(blk->data + localp);
}