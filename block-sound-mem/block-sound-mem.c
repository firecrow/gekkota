#include "block-sound-mem.h"

gka_local_address_t gka_allocate_space(struct gka_mem_block *blk, gka_local_address_t size){
    if(blk->next_available + size > blk->allocated){
            return GKA_BOUNDRY_ACTION; 
    } 
    gka_local_address_t next = blk->next_available;
    blk->next_available += size;
    printf("returning next %d\n", next);
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