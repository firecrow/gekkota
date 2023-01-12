#include "../gekkota.h"

void linear_ease(
    double *progress, struct gka_segment *segment, double start, double end
) {}

void square_ease(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress = start;
}

void ease_in(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress *= (*progress);
}

void ease_flip(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress = 1 - *progress;
}

void ease_out(
    double *progress, struct gka_segment *segment, double start, double end
) {
  double base = 1 - *progress;
  double curved = base * base;
  *progress *= 1 - curved;
}

void ease_inout(
    double *progress, struct gka_segment *segment, double start, double end
) {
  *progress *= (*progress);
}

gka_segment::gka_segment(
    gka_timeint gotime, double start_value, get_ease_func ease
) {
  this->gotime = gotime;
  this->start_value = start_value;
  this->ease = ease;
  this->next = nullptr;
}

gka_segpattern::gka_segpattern(struct gka_segment *root) { this->root = root; }
void gka_segpattern::add_segment(struct gka_segment *seg) {
  if (root == NULL) {
    root = seg;
  } else {
    gka_segment *next = root;
    gka_segment *current = next;
    while (next) {
      current = next;
      next = next->next;
    }
    current->next = seg;
  }
}

struct gka_segment *segment_from_segment(
    struct gka_segpattern *pattern, double base_value, gka_timeint offset
) {
  struct gka_segment *segment = pattern->root;
  while (segment && segment->next && (offset > segment->next->gotime)) {
    segment = segment->next;
  }
  return segment;
}

double value_from_segment(
    struct gka_segpattern *pattern, double base_value, gka_timeint offset
) {

  struct gka_segment *segment =
      segment_from_segment(pattern, base_value, offset);

  double next_value = segment->start_value;
  if (segment && segment->next) {
    next_value = segment->next->start_value;
  }

  if (!segment->next) {
    return base_value * segment->start_value;
  }

  double progress = (double)(offset - segment->gotime) /
                    (double)(segment->next->gotime - segment->gotime);

  segment->ease(
      &progress, segment, segment->start_value, segment->next->start_value
  );

  // debug output
  if (0) {
    printf(
        "\x1b[34moffset %d,progress %lf, between %d/%lf and %d/%lf\x1b[0m",
        offset, progress, segment->gotime, segment->start_value,
        segment->next->gotime, segment->next->start_value
    );
  }

  double segment_value =
      segment->start_value + (next_value - segment->start_value) * progress;

  return base_value * segment_value;
}
