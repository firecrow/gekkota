#include "plot-visual-pre.h"
using namespace std;
#include "plot-visual.h"
void GkaCrosshair::draw(GkaChart *chart, PlotData *data) {
  BLGradient gradient(BLLinearGradientValues(0, 0, 0, chart->width));
  gradient.addStop(0.0, BLRgba32(0xFFFFFFFF));
  gradient.addStop(1.0, BLRgba32(0xFFFFFF00));

  BLPath path;
  path.moveTo(10.0, 10.0);
  for (gka_tv_datapoint *d : data->points) {
    gka_vec2 start, end;
    chart->mapToLocal((double)d->time, d->start, start);
    chart->mapToLocal((double)d->time, d->end, end);

    if (0) {
      printf(
          "%ld, %lf %lf: %lf %lf , %lf %lf\n", d->time, d->start, d->end,
          start.a, start.b, end.a, end.b
      );
    }

    path.moveTo(start.a, 0.0);
    path.lineTo(start.a, chart->height);

    path.moveTo(0.0, start.b);
    path.lineTo(chart->width, start.b);
  }
  chart->uictx->setCompOp(BL_COMP_OP_SRC_OVER);
  chart->uictx->setStrokeStyle(gradient);
  chart->uictx->setStrokeWidth(2);
  chart->uictx->setStrokeStartCap(BL_STROKE_CAP_BUTT);
  chart->uictx->setStrokeEndCap(BL_STROKE_CAP_BUTT);
  chart->uictx->strokePath(path);
}