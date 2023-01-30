#include <gtest/gtest.h>
#include <inttypes.h>

using namespace std;
#include "gekkota-external.h"
#include "time/gka_time.h"
#include "../common/test_common.h"
extern "C" {
#include "sound/sound.h"
}
#include "../debug.c"

namespace GekkotaTest {

class GkaSoundFixture : public testing::Test {
protected:
  GkaSoundFixture() {}
  static gka_entry *MakeExampleSoundEvent(
      struct gka_entry *m, gka_time_t start, gka_decimal_t basevol
  ) {

    gka_local_address_t vol_pattern_seg =
        gka_segment_create(m, 0, basevol + 0.1, GKA_LINEAR);

    gka_segpattern_add_segment_values(
        m, vol_pattern_seg, 10, basevol + 0.1, GKA_LINEAR
    );
    gka_segpattern_add_segment_values(
        m, vol_pattern_seg, 15, basevol + 0.0, GKA_LINEAR
    );

    gka_local_address_t freq_pattern_seg =
        gka_segment_create(m, 0, 440.0, GKA_CLIFF);

    gka_local_address_t soundlp =
        gka_sound_create(m, freq_pattern_seg, vol_pattern_seg);

    gka_local_address_t eventlp = gka_sound_event_create(m, soundlp, start, 0);

    return gka_pointer(m, eventlp);
  }
};

TEST_F(GkaSoundFixture, TestPhaseIncrement) {
  struct gka_entry *m = gka_alloc_memblock(16 * sizeof(struct gka_entry));
  struct gka_entry *event = GkaSoundFixture::MakeExampleSoundEvent(m, 0, 0.0);

  gka_entry *s_alpha = gka_pointer(m, event->values.event.sounds);
  EXPECT_EQ(s_alpha->values.sound.phase, 0.0) << "step starts out as zego";

  gka_get_frame_value_from_event(m, event, 0, 0, 192000);

  double step = s_alpha->values.sound.step;
  EXPECT_EQ(s_alpha->values.sound.phase, step)
      << "step adjusts with each frame requested";

  double originalStep = step;
  struct gka_entry *freq = gka_pointer(m, s_alpha->values.sound.freq);
  double originalFreq = freq->values.segment.value;

  // increasing the frequency
  freq->values.segment.value += 10.0;
  gka_get_frame_value_from_event(m, event, 0, 2, 192000);

  EXPECT_GT(s_alpha->values.sound.step, originalStep)
      << "expect a higher step with an increased freqeuncy";
}

TEST_F(GkaSoundFixture, TestClimbThroughSounds) {
  struct gka_entry *m = gka_alloc_memblock(32 * GKA_SEGMENT_SIZE);
  struct gka_entry *event1 = GkaSoundFixture::MakeExampleSoundEvent(m, 1, 0.0);
  struct gka_entry *event2 =
      GkaSoundFixture::MakeExampleSoundEvent(m, 100, 0.5);
  test_print_mem_block(m);
}

} // namespace GekkotaTest
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}