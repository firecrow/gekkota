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
  bool mapToLocal(double time, double value, gka_vec2 &coord);
  bool boundsCheckToLocal(int *coord[2]);
  void setDimensions(int height, int width);
  void setRanges(
      double range_start_x, double range_end_x, double range_start_y,
      double range_end_y
  );

  GkaChart(BLContext *uictx);
};

/* a class for any time of visual artifact on the visualzation*/
class GkaChartDetail {
public:
  GkaChartDetail();
  virtual void draw(GkaChart *chart, PlotData *data);
};

class GkaPlot : public GkaChartDetail {
public:
  virtual void draw(GkaChart *chart, PlotData *data);
};

class GkaCrosshair : public GkaChartDetail {
public:
  virtual void draw(GkaChart *chart, PlotData *data);
};