#include <stdio.h>
#include <string.h>
#include <sys/types.h>

enum gka_ease { NO_EASE = 0, EASE_IN, EASE_IN_OUT, EASE_OUT };

typedef struct gka_segment (*get_next_segment_func
)(struct gka_segpattern *pattern, struct gka_segment *current);

typedef void (*get_ease_func
)(double *progress, struct gka_segment *segment, double start, double end);

struct gka_segment {
  gka_timeint gotime;
  double start_value;
  struct gka_segment *next;
  get_ease_func ease;
  gka_segment(gka_timeint gotime, double start_value, get_ease_func ease);
};

struct gka_segpattern {
  struct gka_segment *root;
  gka_segpattern(gka_segment *_root);
  void add_segment(gka_segment *seg);
};

void square_ease(
    double *progress, struct gka_segment *segment, double start, double end
);
void linear_ease(
    double *progress, struct gka_segment *segment, double start, double end
);
void ease_flip(
    double *progress, struct gka_segment *segment, double start, double end
);
void ease_in(
    double *progress, struct gka_segment *segment, double start, double end
);
void ease_out(
    double *progress, struct gka_segment *segment, double start, double end
);
void ease_inout(
    double *progress, struct gka_segment *segment, double start, double end
);
struct gka_segment *gka_segment_alloc(
    double start, double start_value, double end_value, get_ease_func ease
);
struct gka_segpattern *
gka_pattern_alloc(size_t length, struct gka_segment *segments[]);

double value_from_segment(
    struct gka_segpattern *pattern, double base_value, gka_timeint offset
);

struct gka_segment *segment_from_segment(
    struct gka_segpattern *pattern, double base_value, double offset
);