#include <gtest/gtest.h>
#include <inttypes.h>

using namespace std;
#include "gekkota-external.h"
#include "../common/test_common.h"

extern "C" {
#include "block-sound-mem/block-sound-mem.h"
#include "segpattern.h"
}

namespace GekkotaTest {

class GkaSegPatternFixture : public testing::Test {
protected:
  GkaSegPatternFixture() {}
};
TEST_F(GkaSegPatternFixture, MemBlockTests) {
  cout << "\x1b[33mrunning test\x1b[0m" << endl;

  char id1 = 1;
  char id2 = 2;
  char id3 = 3;

  typedef char chars24arr[24];

  // clang-format off
  char threeSegs[] = {
    id1,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    id2,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,

    id3,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
  };
  // clang-format on

  struct gka_mem_block _m;
  _m.data = (struct gka_mem_block *)&threeSegs;
  _m.allocated = sizeof(struct gka_mem_block) * 3;
  _m.next_available = 0;
  struct gka_mem_block *m = &_m;

  gka_local_address_t t;
  struct gka_entry *s, *n, *n2;

  cout << "\x1b[34malloc\x1b[0m" << endl;
  t = gka_allocate_space(m, (gka_local_address_t)sizeof(struct gka_entry));
  EXPECT_EQ(t, 0) << "address should be the first one";

  cout << "\x1b[34mpointer\x1b[0m" << endl;
  s = gka_pointer(m, t);
  EXPECT_EQ(
      s->type, id1
  ) << "Expect the first block to have the id of the first test data";

  cout << "\x1b[34mnext allocated pointer\x1b[0m" << endl;
  EXPECT_EQ(m->next_available, gka_to_local(m, s) + sizeof(struct gka_entry))
      << "Expect the next available to be the next neighbor";

  cout << "\x1b[34mnext allocated pointer\x1b[0m" << endl;
}

TEST_F(GkaSegPatternFixture, SegmentCreateTests) {
  cout << "\x1b[33mrunning seg test\x1b[0m" << endl;

  struct gka_mem_block *m = gka_alloc_memblock(6 * sizeof(struct gka_entry));

  gka_local_address_t t;
  struct gka_entry *s, *n, *n2;

  cout << "\x1b[34mmalloc\x1b[0m" << endl;
  t = gka_segment_create(m, 0, 0.0, GKA_LINEAR);
  EXPECT_EQ(t, 24) << "Expect first record be 1 records worth of bytes in";

  s = gka_pointer(m, t);
  EXPECT_EQ(s->type, GKA_SEGMENT_VALUE) << "Expect first record k";
}

TEST_F(GkaSegPatternFixture, PatternCreateTests) {}

/*
TEST_F(GkaSegPatternFixture, AddSegsToPattern) {
  gka_segment *s_alpha = new gka_segment(0, 0.1, &linear_ease);
  EXPECT_EQ(s_alpha->gotime, 0) << "gotime set from constructor";
  EXPECT_EQ(s_alpha->start_value, 0.1) << "start_value set from constructor";
  EXPECT_EQ(s_alpha->ease, &linear_ease) << "ease set from constructor";

  gka_segment *s_bravo = new gka_segment(10, 0.5, &square_ease);
  EXPECT_EQ(s_bravo->gotime, 10) << "gotime set from constructor";
  EXPECT_EQ(s_bravo->start_value, 0.5) << "start_value set from constructor";
  EXPECT_EQ(s_bravo->ease, &square_ease) << "ease set from constructor";

  gka_segpattern *p_alpha = new gka_segpattern(s_alpha);
  p_alpha->add_segment(s_bravo);
  EXPECT_EQ(s_alpha->next, s_bravo) << "next is set by add segment";

  gka_segment *s_charlie = new gka_segment(20, 0.7, &ease_in);
  EXPECT_EQ(s_charlie->gotime, 20) << "gotime set from constructor";
  EXPECT_EQ(s_charlie->start_value, 0.7) << "start_value set from constructor";
  EXPECT_EQ(s_charlie->ease, &ease_in) << "ease set from constructor";

  p_alpha->add_segment(s_charlie);
  EXPECT_EQ(s_bravo->next, s_charlie) << "next is set by add segment";
}

TEST_F(GkaSegPatternFixture, GetSegsFromPattern) {
  gka_segment *s_alpha = new gka_segment(0, 0.1, &linear_ease);
  gka_segment *s_bravo = new gka_segment(10, 0.5, &square_ease);
  gka_segpattern *p_alpha = new gka_segpattern(s_alpha);
  p_alpha->add_segment(s_bravo);
  gka_segment *s_charlie = new gka_segment(20, 0.7, &ease_in);
  p_alpha->add_segment(s_charlie);

  EXPECT_EQ(segment_from_segment(p_alpha, 0), s_alpha)
      << "get relevant segment by time";
  EXPECT_EQ(segment_from_segment(p_alpha, 11), s_bravo)
      << "get relevant segment by time";
  EXPECT_EQ(segment_from_segment(p_alpha, 15), s_bravo)
      << "get relevant segment by time";
  EXPECT_EQ(segment_from_segment(p_alpha, 25), s_charlie)
      << "get relevant segment by time";
}

TEST_F(GkaSegPatternFixture, TestSegPatternValues) {
  gka_segment *s_alpha = new gka_segment(0, 0.1, &linear_ease);
  gka_segment *s_bravo = new gka_segment(10, 0.5, &square_ease);
  gka_segpattern *p_alpha = new gka_segpattern(s_alpha);
  p_alpha->add_segment(s_bravo);
  gka_segment *s_charlie = new gka_segment(20, 0.7, &ease_in);
  p_alpha->add_segment(s_charlie);

  double val;
  val = value_from_segment(p_alpha, 1.0, 0);
  EXPECT_TRUE(FuzzyMatch(val, 0.1, 0.0)) << "compare first value generated";

  val = value_from_segment(p_alpha, 1.0, 5);
  EXPECT_TRUE(val > s_alpha->start_value && val < s_bravo->start_value)
      << "expect value generated between the first two segments to have a "
         "value that is between them";

  val = value_from_segment(p_alpha, 1.0, 10);
  EXPECT_TRUE(FuzzyMatch(val, s_bravo->start_value, 0.0))
      << "expect the value at the start of the second segment to that the "
         "second segments start value";

  val = value_from_segment(p_alpha, 1.0, 25);
  EXPECT_TRUE(FuzzyMatch(val, s_charlie->start_value, 0.0))
      << "expect the value of a segment with no segment after it to match it's "
         "start value";
}
*/

} // namespace GekkotaTest
int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}