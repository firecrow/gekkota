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
  static GkaSoundEvent *MakeExampleSoundEvent() {
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

    return new GkaSoundEvent({sound}, (gka_timeint)0);
  }
};

TEST_F(GkaSoundFixture, TestSound) { GkaSoundFixture::MakeExampleSoundEvent(); }

TEST_F(GkaSoundFixture, TestPhaseIncrement) {
  GkaSoundEvent *soundEvent = GkaSoundFixture::MakeExampleSoundEvent();
  GkaSound *s_alpha = soundEvent->soundg[0];
  EXPECT_EQ(s_alpha->phase, 0) << "step starts out as zego";

  soundEvent->getFrameValue(0, 0, 192000);

  double step = s_alpha->step;

  soundEvent->getFrameValue(0, 1, 192000);

  EXPECT_EQ(s_alpha->phase, step * 2)
      << "step adjusts with each frame requested";

  double originalStep = step;
  double originalFreq = s_alpha->freq->root->start_value;
  // increasing the frequency
  s_alpha->freq->root->start_value += 10.0;
  soundEvent->getFrameValue(0, 2, 192000);

  EXPECT_GT(s_alpha->step, originalStep)
      << "expect a higher step with an increased freqeuncy";
}

} // namespace GekkotaTest
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}