#include <gtest/gtest.h>
#include <inttypes.h>

using namespace std;
#include "gekkota-external.h"
#include "gka_time.h"
#include "segpattern.h"

namespace GekkotaTest {

class GkaSegPatternFixture : public testing::Test {
protected:
  GkaSegPatternFixture() {}
};

TEST_F(GkaSegPatternFixture, SetupParams) {
  // test isntantiation
  gka_segment *s_alpha = new gka_segment(0, 0.1, &linear_ease);
  EXPECT_EQ(s_alpha->gotime, 0);
  EXPECT_EQ(s_alpha->start_value, 0.1);
  EXPECT_EQ(s_alpha->ease, &linear_ease);
}
} // namespace GekkotaTest
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}