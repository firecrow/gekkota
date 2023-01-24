#include "gka_debug.h"

static char *type_strings[] = {
    "GKA_UNSPECIFIED",
    "GKA_HEAD",
    "GKA_RESERVED_BY_NEIGHBOUR",
    "GKA_SOUND",
    "GKA_SOUND_EVENT",
    "GKA_END"
    "GKA_SOUND_EVENT",
    "GKA_SEGMENT_VALUE",
    "GKA_NEXT_LOCAL"};

void test_print_entry(struct gka_entry *e) {

  if (e == NULL) {
    printf("NULL for segment argument");
    fflush(stdout);
    return;
  }

  printf("%s", type_strings[e->values.all.type], e->values.segment.value);
  if (e->values.all.type == GKA_NEXT_LOCAL) {
    printf(" -> %ld", e->values.link.addr / GKA_SEGMENT_SIZE);
  } else if (e->values.all.type == GKA_SEGMENT_VALUE) {
    printf(" = %ld:%lf", e->values.segment.start_time, e->values.segment.value);
  } else if (e->values.all.type == GKA_SOUND) {
    printf(
        " freq -> %ld, volume -> %ld, phase:%lf",
        e->values.sound.freq / GKA_SEGMENT_SIZE,
        e->values.sound.volume / GKA_SEGMENT_SIZE, e->values.sound.phase
    );
  } else if (e->values.all.type == GKA_SOUND_EVENT) {
    printf(
        " sounds -> %ld, start:%ld, repeat:%ld",
        e->values.event.sounds / GKA_SEGMENT_SIZE, e->values.event.start,
        e->values.event.repeat
    );
  }
}

void test_print_mem_block(struct gka_entry *blk) {
  printf("m(%ld/%ld) ->\n", blk->values.head.next_available, blk->values.head.allocated);

  gka_local_address_t i = 0;
  while (i < blk->values.head.allocated) {
    printf("%02d ", i / GKA_SEGMENT_SIZE);
    test_print_entry(gka_pointer(blk, i));
    printf("\n");
    i += GKA_SEGMENT_SIZE;
  }
}