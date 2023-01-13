#include <gtest/gtest.h>
#include <inttypes.h>

using namespace std;
#include "gekkota-external.h"
#include "gka_time.h"
#include "../common/test_common.h"
#include "segpattern.h"
#include "sound.h"

namespace GekkotaTest {

class GkaSoundFixture : public testing::Test {
protected:
  GkaSoundFixture() {}
};

TEST_F(GkaSoundFixture, TestSound) {

  gka_segment *vol_s_alpha = new gka_segment(0, 0.1, &linear_ease);
  gka_segment *vol_s_bravo = new gka_segment(10, 0.1, &linear_ease);
  gka_segment *vol_s_charlie = new gka_segment(15, 0.0, &ease_out);

  gka_segpattern *volptn = new gka_segpattern(vol_s_alpha);
  volptn->add_segment(vol_s_bravo);
  volptn->add_segment(vol_s_charlie);

  gka_segment *freq_s_alpha = new gka_segment(0, 440.0, &square_ease);
  gka_segpattern *freqptn = new gka_segpattern(freq_s_alpha);

  gka_segment *dist_s_alpha = new gka_segment(0, 0.0, &square_ease);
  gka_segpattern *distptn = new gka_segpattern(dist_s_alpha);

  GkaSound *sound = new GkaSound(freqptn, volptn, distptn);

  GkaSoundEvent *soundEvent = new GkaSoundEvent({sound}, (gka_timeint)0);
}

} // namespace GekkotaTest
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}