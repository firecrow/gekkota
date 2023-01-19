//#include "../block-sound-mem/block-sound-mem.h"
//#include "../segpattern/segpattern.h"

static char *type_strings[] = {"GKA_UNSPECIFIED",   "GKA_RESERVED_BY_NEIGHBOUR",
                               "GKA_SOUND",         "GKA_END",
                               "GKA_SEGMENT_VALUE", "GKA_NEXT_LOCAL"};

void test_print_entry(struct gka_entry *e) {
  if (e == NULL) {
    printf("NULL for segment argument");
    return;
  }
  printf("%s", type_strings[e->type], e->values.segment.value);
  if (e->type == GKA_NEXT_LOCAL || e->type == GKA_SOUND) {
    printf(" -> %ld", e->values.link.addr / sizeof(struct gka_entry));
  } else if (e->type == GKA_SEGMENT_VALUE) {
    printf(" = %ld:%lf", e->values.segment.start_time, e->values.segment.value);
  }
  printf("");
}

void test_print_mem_block(struct gka_mem_block *blk) {
  printf("m(%ld/%ld) ->\n", blk->next_available, blk->allocated);

  gka_local_address_t i = 0;
  while (i < blk->allocated) {
    printf("%02d ", i / sizeof(struct gka_entry));
    test_print_entry(gka_pointer(blk, i));
    printf("\n");
    i += sizeof(struct gka_entry);
  }
  printf("<-\n", blk->next_available, blk->allocated);
}