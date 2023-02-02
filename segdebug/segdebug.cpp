#include "../gekkota.h"

using namespace std;

void tear_down(int sig) {
  FrontEndService::getInstance()->running = false;
  FrontEndService::getInstance()->teardown();
}

/**
 * This is a debugging application for visualizing the segment data using
 * plot-visual
 */
void plot_seg_example(FrontEndService *fe) {
  gka_segment *segs[2];

  int diagram_width = 50;

  gka_segpattern *seg = new gka_segpattern(new gka_segment(0, 0.0, &ease_out));
  seg->add_segment(new gka_segment(30, 1.0, &ease_in));
  seg->add_segment(new gka_segment(100, 0.5, &ease_out));

  PlotData data;
  gka_tv_datapoint xhpoint;
  PlotData xyhair;
  xhpoint.end = xhpoint.start = xhpoint.start = xhpoint.time = 0;
  xyhair.points.push_back(&xhpoint);

  struct gka_tv_datapoint *prev = nullptr;
  for (double i = 0; i < 100; i += 1) {
    double v = value_from_segment(seg, 1.0, i);

    struct gka_tv_datapoint *d = new gka_tv_datapoint();
    d->start = 0; // overwritten by an existing previous event

    d->end = v;
    d->time = i;
    d->meta = 0;

    if (prev) {
      d->start = prev->end;
      data.points.push_back(d);
    }
    prev = d;
  }
  GkaChart *chart = new GkaChart(fe->uictx);
  chart->setDimensions(480, 1024);
  chart->setRanges(0, 100, 1.0, -1.0);

  GkaPlot plot;
  plot.draw(chart, &data);

  GkaCrosshair crosshair;
  crosshair.draw(chart, &xyhair);

  delete chart;
}

int main() {
  FrontEndService *fe = FrontEndService::getInstance();

  fe->Init();

  plot_seg_example(fe);
  fe->commit();

  fe->loop();

  signal(SIGINT, tear_down);
}
