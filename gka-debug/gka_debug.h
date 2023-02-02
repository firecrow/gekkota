#include "../audio-segment/audio-segment.h"
/**
 * This function prints out an entry, with it's header and expected values
 */
void test_print_entry(struct gka_entry *e);
/**
 * This prints an entire mem block with all of it's entries including numerical
 * links to each segment/sound/event so that the state of the memory block can
 * be observed
 */
void test_print_mem_block(struct gka_entry *blk);