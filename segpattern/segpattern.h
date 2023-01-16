#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

enum gka_ease { NO_EASE = 0, EASE_IN, EASE_IN_OUT, EASE_OUT };

typedef struct gka_entry (*get_next_segment_func
)(struct gka_segpattern *pattern, struct gka_entry *current);

typedef void (*get_ease_func
)(double *progress, struct gka_entry *segment, double start, double end);

struct gka_segpattern_pattern {
  struct gka_entry *root;
  struct gka_mem_block *blk;
};

void gka_segpattern_add_segment(
    struct gka_mem_block *blk, struct gka_entry *pattern, struct gka_entry *seg
);

int gka_entry_create(
    struct gka_mem_block *blk, gka_value_t gotime, gka_decimal_t start_value,
    gka_operand_t ease
);
void gka_segpattern(struct gka_entry *_root);

void square_ease(
    double *progress, struct gka_entry *segment, double start, double end
);
void linear_ease(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_flip(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_in(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_out(
    double *progress, struct gka_entry *segment, double start, double end
);
void ease_inout(
    double *progress, struct gka_entry *segment, double start, double end
);
struct gka_entry *gka_segment_alloc(
    double start, double start_value, double end_value, get_ease_func ease
);
struct gka_segpattern *
gka_pattern_alloc(size_t length, struct gka_entry *segments[]);

double value_from_segment(
    struct gka_mem_block *blk, struct gka_segpattern *pattern,
    gka_decimal_t base_value, gka_time_t offset
);

struct gka_entry *segment_from_segment(
    struct gka_mem_block *blk, struct gka_entry *pattern, gka_time_t offset
);