/**
 * This is the initial data point structure used to visualize time series events
 *
 * TODO: use a more basic structure for events directly, and move this to a time
 * series specific abstration
 */
struct gka_tv_datapoint {
  uint64_t time;
  double start;
  double end;
  uint64_t meta;
};

struct PlotData {
  vector<gka_tv_datapoint *> points;
};

struct gka_vec2 {
  double a;
  double b;
};

/**
 * This is a 2D vector based chart which does calculations to map ranges in
 * proportion.
 *
 * Design to be something that can give the effect of zooming or focusing in/out
 * on specific data ranges.
 */
class GkaChart {
public:
  BLContext *uictx;
  int height;
  int width;
  double range_start_x;
  double range_end_x;
  double range_start_y;
  double range_end_y;
  double range_length_x;
  double range_length_y;
  /**
   * This is a function which maps a value into the local positioning of that
   * value, given the range and bounds of the chart
   */
  bool mapToLocal(double time, double value, gka_vec2 &coord);
  bool boundsCheckToLocal(int *coord[2]);
  /**
   * This configures the dimensions of the chart, this can be done after
   * instantiation to allow for configuration of the existing object
   */
  void setDimensions(int height, int width);
  /**
   * sets the `virtual` range of the chart which is used for mapping to
   * determing the visual cooridnates of data
   */
  void setRanges(
      double range_start_x, double range_end_x, double range_start_y,
      double range_end_y
  );

  /**
   * This is the constructor which requires a Blend2D context which will be used
   * to draw the results into
   */
  GkaChart(BLContext *uictx);
};

/**
 * This serves as a base  class for any time of visual artifact on the
 * visualzation
 */
class GkaChartDetail {
public:
  GkaChartDetail();
  virtual void draw(GkaChart *chart, PlotData *data);
};

/**
 * This is the plot visual, usually for time series data, used to draw onto a
 * GkaChart object
 */
class GkaPlot : public GkaChartDetail {
public:
  virtual void draw(GkaChart *chart, PlotData *data);
};

/**
 * This is a crosshair object, usually used for axis labeling purposes, it draws
 * a line across the +/- x and y planes, and does not always occur at 0,0
 */
class GkaCrosshair : public GkaChartDetail {
public:
  virtual void draw(GkaChart *chart, PlotData *data);
};