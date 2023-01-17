//#include "../block-sound-mem/block-sound-mem.h"
//#include "../segpattern/segpattern.h"

static char *type_strings[] = {"GKA_UNSPECIFIED",   "GKA_RESERVED_BY_NEIGHBOUR",
                               "GKA_SOUND",         "GKA_END",
                               "GKA_SEGMENT_VALUE", "GKA_NEXT_LOCAL"};

void test_print_entry(struct gka_entry *e) {
  printf("t(%s:%lf)", type_strings[e->type], e->values.placement.value);
}

void test_print_mem_block(struct gka_mem_block *blk) {
  printf("m(%ld/%ld) ->\n", blk->next_available, blk->allocated);

  gka_local_address_t i = 0;
  while (i < blk->allocated) {
    test_print_entry(gka_pointer(blk, i));
    printf(",\n");
    i += sizeof(struct gka_entry);
  }
  printf("<-\n", blk->next_available, blk->allocated);
}