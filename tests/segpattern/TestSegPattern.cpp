#include <gtest/gtest.h>
#include <inttypes.h>

using namespace std;
#include "gekkota-external.h"
#include "../common/test_common.h"

extern "C" {
#include "block-sound-mem.h"
#include "segpattern.h"
#include "../debug.c"
}

namespace GekkotaTest {

/* two patterns is enough to test overlap and collisions*/
class TwoPatternBlock {
public:
  gka_local_address_t p1;
  gka_local_address_t p2;
  struct gka_entry *m;

  TwoPatternBlock(
      struct gka_entry *m, gka_local_address_t p1, gka_local_address_t p2
  ) {
    this->m = m;
    this->p1 = p1;
    this->p2 = p2;
  }
};

class GkaSegPatternFixture : public testing::Test {
public:
  static TwoPatternBlock *generateBasicBlockOfSegments() {
    gka_time_t START = 0;
    gka_decimal_t VALUE = 0.5;
    gka_operand_t TRANSITION = GKA_CLIFF;

    gka_time_t START2 = 100;
    gka_decimal_t VALUE2 = 1.0;
    gka_operand_t TRANSITION2 = GKA_LINEAR;

    gka_time_t START3 = 200;
    gka_decimal_t VALUE3 = 1.5;
    gka_operand_t TRANSITION3 = GKA_LINEAR;

    gka_time_t START4 = 300;
    gka_decimal_t VALUE4 = 1.75;
    gka_operand_t TRANSITION4 = GKA_LINEAR;

    gka_time_t START5 = 400;
    gka_decimal_t VALUE5 = 1.85;
    gka_operand_t TRANSITION5 = GKA_LINEAR;

    gka_time_t START6 = 500;
    gka_decimal_t VALUE6 = 1.95;
    gka_operand_t TRANSITION6 = GKA_LINEAR;

    gka_time_t START7 = 600;
    gka_decimal_t VALUE7 = 1.99;
    gka_operand_t TRANSITION7 = GKA_LINEAR;

    // second pattern tests
    gka_time_t P2_START = 0;
    gka_decimal_t P2_VALUE = 2.0;
    gka_operand_t P2_TRANSITION = GKA_CLIFF;

    gka_time_t P2_START2 = 100;
    gka_decimal_t P2_VALUE2 = 2.1;
    gka_operand_t P2_TRANSITION2 = GKA_LINEAR;

    gka_time_t P2_START3 = 200;
    gka_decimal_t P2_VALUE3 = 2.5;
    gka_operand_t P2_TRANSITION3 = GKA_LINEAR;

    gka_time_t P2_START4 = 300;
    gka_decimal_t P2_VALUE4 = 2.75;
    gka_operand_t P2_TRANSITION4 = GKA_LINEAR;

    struct gka_entry *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));

    struct gka_entry _s;
    _s.values.segment.start_time = START2;
    _s.values.segment.value = VALUE2;
    _s.values.segment.transition = TRANSITION2;

    gka_local_address_t p1s0 =
        gka_segment_create(m, START2, VALUE2, TRANSITION2);

    _s.values.segment.start_time = START3;
    _s.values.segment.value = VALUE3;
    _s.values.segment.transition = TRANSITION3;
    gka_segpattern_add_segment(m, p1s0, &_s);

    _s.values.segment.start_time = START4;
    _s.values.segment.value = VALUE4;
    _s.values.segment.transition = TRANSITION4;
    gka_segpattern_add_segment(m, p1s0, &_s);

    // start adding a second pattern to the memblock
    gka_local_address_t p2s0 =
        gka_segment_create(m, P2_START, P2_VALUE, P2_TRANSITION);

    _s.values.segment.start_time = P2_START2;
    _s.values.segment.value = P2_VALUE2;
    _s.values.segment.transition = P2_TRANSITION2;
    gka_segpattern_add_segment(m, p2s0, &_s);

    _s.values.segment.start_time = P2_START3;
    _s.values.segment.value = P2_VALUE3;
    _s.values.segment.transition = P2_TRANSITION3;
    gka_segpattern_add_segment(m, p2s0, &_s);

    _s.values.segment.start_time = P2_START4;
    _s.values.segment.value = P2_VALUE4;
    _s.values.segment.transition = P2_TRANSITION4;
    gka_segpattern_add_segment(m, p2s0, &_s);

    // add more back from the original pattern now to cause a segment
    _s.values.segment.start_time = START5;
    _s.values.segment.value = VALUE5;
    _s.values.segment.transition = TRANSITION5;
    gka_segpattern_add_segment(m, p1s0, &_s);

    _s.values.segment.start_time = START6;
    _s.values.segment.value = VALUE6;
    _s.values.segment.transition = TRANSITION6;
    gka_segpattern_add_segment(m, p1s0, &_s);

    _s.values.segment.start_time = START7;
    _s.values.segment.value = VALUE7;
    _s.values.segment.transition = TRANSITION7;
    gka_segpattern_add_segment(m, p1s0, &_s);

    return new TwoPatternBlock(m, p1s0, p2s0);
  }

protected:
  GkaSegPatternFixture(){};
};

TEST_F(GkaSegPatternFixture, EntrySizeIsAccurateTests) {
  EXPECT_EQ(sizeof(struct gka_entry), GKA_SEGMENT_SIZE);
}

TEST_F(GkaSegPatternFixture, MemBlockTests) {

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

  struct gka_entry _m;
  _m.data = (struct gka_entry *)&threeSegs;
  _m.allocated = sizeof(struct gka_entry) * 3;
  _m.next_available = 0;
  struct gka_entry *m = &_m;

  gka_local_address_t t;
  struct gka_entry *s, *n, *n2;

  t = gka_allocate_space(m, (gka_local_address_t)sizeof(struct gka_entry));
  EXPECT_EQ(t, 0) << "address should be the first one";

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.all.type, id1)
      << "Expect the first block to have the id of the first test data";

  EXPECT_EQ(m->next_available, gka_to_local(m, s) + sizeof(struct gka_entry))
      << "Expect the next available to be the next neighbor";
}

TEST_F(GkaSegPatternFixture, SegmentCreateTests) {

  struct gka_entry *m = gka_alloc_memblock(6 * sizeof(struct gka_entry));

  gka_local_address_t t;
  struct gka_entry *s, *n, *n2;

  t = gka_segment_create(m, 0, 0.0, GKA_LINEAR);
  EXPECT_EQ(t, GKA_SEGMENT_SIZE)
      << "Expect first record be 1 records worth of bytes in";

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.all.type, GKA_SEGMENT_VALUE) << "Expect first record k";
}

TEST_F(GkaSegPatternFixture, ExtendSegmentTests) {
  gka_time_t START = 0;
  gka_decimal_t VALUE = 0.5;
  gka_operand_t TRANSITION = GKA_CLIFF;

  gka_time_t START2 = 100;
  gka_decimal_t VALUE2 = 1.0;
  gka_operand_t TRANSITION2 = GKA_LINEAR;

  gka_time_t START3 = 200;
  gka_decimal_t VALUE3 = 1.5;
  gka_operand_t TRANSITION3 = GKA_LINEAR;

  struct gka_entry *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));

  gka_local_address_t t = gka_segment_create(m, START, VALUE, TRANSITION);

  struct gka_entry _s;
  _s.values.segment.start_time = START2;
  _s.values.segment.value = VALUE2;
  _s.values.segment.transition = TRANSITION2;
  gka_local_address_t t2 = gka_extend_segment(m, t, &_s);

  struct gka_entry *s2 = gka_pointer(m, t2);
  EXPECT_EQ(s2->values.segment.start_time, START2);
  EXPECT_EQ(s2->values.segment.value, VALUE2);
  EXPECT_EQ(s2->values.segment.transition, TRANSITION2);

  struct gka_entry _s2;
  _s2.values.segment.start_time = START3;
  _s2.values.segment.value = VALUE3;
  _s2.values.segment.transition = TRANSITION3;
  gka_local_address_t t3 = gka_extend_segment(m, t2, &_s2);

  struct gka_entry *s3 = gka_pointer(m, t3);
  EXPECT_EQ(s3->values.segment.start_time, START3);
  EXPECT_EQ(s3->values.segment.value, VALUE3);
  EXPECT_EQ(s3->values.segment.transition, TRANSITION3);
}

TEST_F(GkaSegPatternFixture, PatternCreateTests) {
  struct gka_entry *m = gka_alloc_memblock(16 * sizeof(struct gka_entry));
  struct gka_entry _s;
  gka_local_address_t t, r;

  gka_time_t START = 0;
  gka_decimal_t VALUE = 0.5;
  gka_operand_t TRANSITION = GKA_CLIFF;

  gka_time_t START2 = 100;
  gka_decimal_t VALUE2 = 1.0;
  gka_operand_t TRANSITION2 = GKA_LINEAR;

  gka_time_t START3 = 200;
  gka_decimal_t VALUE3 = 1.5;
  gka_operand_t TRANSITION3 = GKA_LINEAR;

  gka_time_t START4 = 300;
  gka_decimal_t VALUE4 = 1.75;
  gka_operand_t TRANSITION4 = GKA_LINEAR;

  r = gka_segment_create(m, START2, VALUE2, TRANSITION2);
  t = r;

  gka_segpattern_add_segment(m, r, &_s);
  struct gka_entry *s = gka_pointer(m, t);

  EXPECT_EQ(s->values.segment.start_time, START2);
  EXPECT_EQ(s->values.segment.value, VALUE2);
  EXPECT_EQ(s->values.segment.transition, TRANSITION2);

  _s.values.segment.start_time = START3;
  _s.values.segment.value = VALUE3;
  _s.values.segment.transition = TRANSITION3;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START3);
  EXPECT_EQ(s->values.segment.value, VALUE3);
  EXPECT_EQ(s->values.segment.transition, TRANSITION3);

  _s.values.segment.start_time = START4;
  _s.values.segment.value = VALUE4;
  _s.values.segment.transition = TRANSITION4;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START4);
  EXPECT_EQ(s->values.segment.value, VALUE4);
  EXPECT_EQ(s->values.segment.transition, TRANSITION4);
}
TEST_F(GkaSegPatternFixture, CollisionTests) {

  gka_time_t START = 0;
  gka_decimal_t VALUE = 0.5;
  gka_operand_t TRANSITION = GKA_CLIFF;

  gka_time_t START2 = 100;
  gka_decimal_t VALUE2 = 1.0;
  gka_operand_t TRANSITION2 = GKA_LINEAR;

  gka_time_t START3 = 200;
  gka_decimal_t VALUE3 = 1.5;
  gka_operand_t TRANSITION3 = GKA_LINEAR;

  gka_time_t START4 = 300;
  gka_decimal_t VALUE4 = 1.75;
  gka_operand_t TRANSITION4 = GKA_LINEAR;

  gka_time_t START5 = 400;
  gka_decimal_t VALUE5 = 1.85;
  gka_operand_t TRANSITION5 = GKA_LINEAR;

  gka_time_t START6 = 500;
  gka_decimal_t VALUE6 = 1.95;
  gka_operand_t TRANSITION6 = GKA_LINEAR;

  gka_time_t START7 = 600;
  gka_decimal_t VALUE7 = 1.99;
  gka_operand_t TRANSITION7 = GKA_LINEAR;

  // second pattern tests
  gka_time_t P2_START = 0;
  gka_decimal_t P2_VALUE = 2.5;
  gka_operand_t P2_TRANSITION = GKA_CLIFF;

  gka_time_t P2_START2 = 100;
  gka_decimal_t P2_VALUE2 = 2.0;
  gka_operand_t P2_TRANSITION2 = GKA_LINEAR;

  gka_time_t P2_START3 = 200;
  gka_decimal_t P2_VALUE3 = 2.5;
  gka_operand_t P2_TRANSITION3 = GKA_LINEAR;

  gka_time_t P2_START4 = 300;
  gka_decimal_t P2_VALUE4 = 2.75;
  gka_operand_t P2_TRANSITION4 = GKA_LINEAR;

  struct gka_entry *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));

  struct gka_entry _s;
  gka_local_address_t r = gka_segment_create(m, START2, VALUE2, TRANSITION2);
  gka_local_address_t t;

  struct gka_entry *s = gka_pointer(m, r);
  EXPECT_EQ(s->values.segment.start_time, START2);
  EXPECT_EQ(s->values.segment.value, VALUE2);
  EXPECT_EQ(s->values.segment.transition, TRANSITION2);

  _s.values.segment.start_time = START3;
  _s.values.segment.value = VALUE3;
  _s.values.segment.transition = TRANSITION3;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START3);
  EXPECT_EQ(s->values.segment.value, VALUE3);
  EXPECT_EQ(s->values.segment.transition, TRANSITION3);

  _s.values.segment.start_time = START4;
  _s.values.segment.value = VALUE4;
  _s.values.segment.transition = TRANSITION4;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START4);
  EXPECT_EQ(s->values.segment.value, VALUE4);
  EXPECT_EQ(s->values.segment.transition, TRANSITION4);

  // start adding a second pattern to the memblock

  gka_local_address_t r2 =
      gka_segment_create(m, P2_START, P2_VALUE, P2_TRANSITION);

  s = gka_pointer(m, r2);
  EXPECT_EQ(s->values.segment.start_time, P2_START);
  EXPECT_EQ(s->values.segment.value, P2_VALUE);
  EXPECT_EQ(s->values.segment.transition, P2_TRANSITION);

  _s.values.segment.start_time = P2_START2;
  _s.values.segment.value = P2_VALUE2;
  _s.values.segment.transition = P2_TRANSITION2;
  t = gka_segpattern_add_segment(m, r2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, P2_START2);
  EXPECT_EQ(s->values.segment.value, P2_VALUE2);
  EXPECT_EQ(s->values.segment.transition, P2_TRANSITION2);

  _s.values.segment.start_time = P2_START3;
  _s.values.segment.value = P2_VALUE3;
  _s.values.segment.transition = P2_TRANSITION3;
  t = gka_segpattern_add_segment(m, r2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, P2_START3);
  EXPECT_EQ(s->values.segment.value, P2_VALUE3);
  EXPECT_EQ(s->values.segment.transition, P2_TRANSITION3);

  _s.values.segment.start_time = P2_START4;
  _s.values.segment.value = P2_VALUE4;
  _s.values.segment.transition = P2_TRANSITION4;
  t = gka_segpattern_add_segment(m, r2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, P2_START4);
  EXPECT_EQ(s->values.segment.value, P2_VALUE4);
  EXPECT_EQ(s->values.segment.transition, P2_TRANSITION4);

  // add more back from the original pattern now to cause a segment
  _s.values.segment.start_time = START5;
  _s.values.segment.value = VALUE5;
  _s.values.segment.transition = TRANSITION5;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START5);
  EXPECT_EQ(s->values.segment.value, VALUE5);
  EXPECT_EQ(s->values.segment.transition, TRANSITION5);

  _s.values.segment.start_time = START6;
  _s.values.segment.value = VALUE6;
  _s.values.segment.transition = TRANSITION6;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START6);
  EXPECT_EQ(s->values.segment.value, VALUE6);
  EXPECT_EQ(s->values.segment.transition, TRANSITION6);

  _s.values.segment.start_time = START7;
  _s.values.segment.value = VALUE7;
  _s.values.segment.transition = TRANSITION7;
  t = gka_segpattern_add_segment(m, r, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.segment.start_time, START7);
  EXPECT_EQ(s->values.segment.value, VALUE7);
  EXPECT_EQ(s->values.segment.transition, TRANSITION7);

  test_print_mem_block(m);
}

TEST_F(GkaSegPatternFixture, SegmentFromSegmentTests) {

  TwoPatternBlock *b = GkaSegPatternFixture::generateBasicBlockOfSegments();

  struct gka_entry *ep0_0 = gka_segment_from_pattern(b->m, b->p1, 0);
  struct gka_entry *ep0_100 = gka_segment_from_pattern(b->m, b->p1, 100);
  struct gka_entry *ep0_201 = gka_segment_from_pattern(b->m, b->p1, 201);
  struct gka_entry *ep0_555 = gka_segment_from_pattern(b->m, b->p1, 555);

  EXPECT_EQ(ep0_0->values.segment.value, 1.0);
  EXPECT_EQ(ep0_100->values.segment.value, 1.0);
  EXPECT_EQ(ep0_201->values.segment.value, 1.5);
  EXPECT_EQ(ep0_555->values.segment.value, 1.95);

  struct gka_entry *ep2_100 = gka_segment_from_pattern(b->m, b->p2, 100);
  struct gka_entry *ep2_375 = gka_segment_from_pattern(b->m, b->p2, 375);
  EXPECT_EQ(ep2_100->values.segment.value, 2.0);
  EXPECT_EQ(ep2_375->values.segment.value, 2.75);
}

TEST_F(GkaSegPatternFixture, ValueFromSegmentTests) {
  TwoPatternBlock *b = GkaSegPatternFixture::generateBasicBlockOfSegments();
  test_print_mem_block(b->m);

  EXPECT_EQ(value_from_segment(b->m, b->p1, 1.0, 0), 1.0);
  EXPECT_TRUE(FuzzyMatch(value_from_segment(b->m, b->p1, 1.0, 150), 1.0, 1.5));
  EXPECT_TRUE(FuzzyMatch(value_from_segment(b->m, b->p1, 1.0, 440), 1.85, 1.95)
  );
  EXPECT_EQ(value_from_segment(b->m, b->p1, 1.0, 1000), 1.99);

  EXPECT_EQ(value_from_segment(b->m, b->p2, 1.0, 0), 2.0);
}

} // namespace GekkotaTest

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}