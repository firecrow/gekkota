#include "audio-segment.h"

__PROCESS_BOTH__ void linear_ease(
    double *progress, struct gka_entry *segment, double start, double end
) {}

__PROCESS_BOTH__ void square_ease(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress = start;
}

__PROCESS_BOTH__ void
ease_in(double *progress, struct gka_entry *segment, double start, double end) {
  *progress *= (*progress);
}

__PROCESS_BOTH__ void ease_flip(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress = 1 - *progress;
}

__PROCESS_BOTH__ void ease_out(
    double *progress, struct gka_entry *segment, double start, double end
) {
  double base = 1 - *progress;
  double curved = base * base;
  *progress *= 1 - curved;
}

__PROCESS_BOTH__ void ease_inout(
    double *progress, struct gka_entry *segment, double start, double end
) {
  *progress *= (*progress);
}

__PROCESS_BOTH__ void gka_apply_transition(
    double *progress, gka_operand_t transition, struct gka_entry *segment,
    double start, double end
) {
  switch (transition) {
  case GKA_CLIFF:
    square_ease(progress, segment, start, end);
    break;
  case GKA_LINEAR:
    linear_ease(progress, segment, start, end);
    break;
  case GKA_EASE_IN:
    ease_in(progress, segment, start, end);
    break;
  case GKA_EASE_OUT:
    ease_out(progress, segment, start, end);
    break;
  }
}