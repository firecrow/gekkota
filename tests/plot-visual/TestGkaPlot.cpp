#include <gtest/gtest.h>
#include <blend2d.h>
#include <vector>
#include <inttypes.h>

using namespace std;
#include "plot-visual.h"

namespace GekkotaTest {

class GkaPlotTestFixture : public testing::Test {
protected:
  GkaPlotTestFixture() {}
};

TEST_F(GkaPlotTestFixture, SetupParams) {
  int height = 100;
  int width = 200;
  double start_x = 0;
  double end_x = 100;
  double start_y = -1.0;
  double end_y = 1.0;
  BLContext *mock_uictx;
  GkaChart *chart = new GkaChart(mock_uictx);
  EXPECT_EQ(chart->uictx, mock_uictx) << "Sets context pointer on init";
  chart->setDimensions(height, width);

  EXPECT_EQ(chart->height, height) << "height assignment";
  EXPECT_EQ(chart->width, width) << "width assignment";

  chart->setRanges(start_x, end_x, start_y, end_y);
  EXPECT_EQ(chart->range_start_x, start_x) << "Range start x assignment";
  EXPECT_EQ(chart->range_end_x, end_x) << "Range end x assignment";

  EXPECT_EQ(chart->range_start_y, start_y) << "Range start y assignment";
  EXPECT_EQ(chart->range_end_y, end_y) << "Range end y assignment";

  EXPECT_EQ(chart->range_length_x, 100)
      << "Expect that the plot range x is correctly calculated";

  EXPECT_EQ(chart->range_length_y, 2.0)
      << "Expect that the plot range y is correctly calculated";

  gka_vec2 c;
  chart->mapToLocal(0.0, 0.0, c);
  EXPECT_EQ(c.a, 0.000000) << "X coord";
  EXPECT_EQ(c.b, 50.000000) << "Y coord";

  chart->mapToLocal(10.0, 0.5, c);
  EXPECT_EQ(c.a, 20.0) << "X coord";
  EXPECT_EQ(c.b, 75.0) << "Y coord";

  chart->mapToLocal(10.0, -0.5, c);
  EXPECT_EQ(c.a, 20.0) << "X coord";
  EXPECT_EQ(c.b, 25.0) << "Y coord";

  chart->mapToLocal(30.0, 0.72, c);
  EXPECT_EQ(c.a, 60.0) << "X coord";
  EXPECT_EQ(c.b, 86.0) << "Y coord";

  chart->mapToLocal(100.0, -1.0, c);
  EXPECT_EQ(c.a, 200.0) << "X coord";
  EXPECT_EQ(c.b, 0.000000) << "Y coord";
}
} // namespace GekkotaTest
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}