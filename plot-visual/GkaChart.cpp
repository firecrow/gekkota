#include <inttypes.h>
#include <blend2d.h>
#include <iostream>
#include <vector>
#include "../math/utils_global.h"

using namespace std;
#include "plot-visual.h"

GkaChart::GkaChart(BLContext *ctx) { this->uictx = ctx; }

void GkaChart::setDimensions(int height, int width) {
  this->height = height;
  this->width = width;
}

void GkaChart::setRanges(
    double range_start_x, double range_end_x, double range_start_y,
    double range_end_y
) {
  this->range_start_x = range_start_x;
  this->range_end_x = range_end_x;
  this->range_start_y = range_start_y;
  this->range_end_y = range_end_y;

  this->range_length_x = range_end_x - range_start_x;
  this->range_length_y = range_end_y - range_start_y;
}

bool GkaChart::mapToLocal(double time, double value, gka_vec2 &coord) {
  coord.a =
      position_in_transition(time, this->range_start_x, this->range_end_x) *
      this->width;
  coord.b =
      position_in_transition(value, this->range_start_y, this->range_end_y) *
      this->height;

  return true;
}