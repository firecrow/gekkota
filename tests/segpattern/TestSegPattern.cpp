#include <gtest/gtest.h>
#include <inttypes.h>

using namespace std;
#include "gekkota-external.h"
#include "../common/test_common.h"

extern "C" {
#include "block-sound-mem/block-sound-mem.h"
#include "segpattern.h"
#include "debug.c"
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

  struct gka_mem_block *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));

  gka_local_address_t t = gka_segment_create(m, START, VALUE, TRANSITION);

  struct gka_entry _s;
  _s.values.placement.start_time = START2;
  _s.values.placement.value = VALUE2;
  _s.transition = TRANSITION2;
  gka_local_address_t t2 = gka_extend_segment(m, t, &_s);

  struct gka_entry *s2 = gka_pointer(m, t2);
  EXPECT_EQ(s2->values.placement.start_time, START2);
  EXPECT_EQ(s2->values.placement.value, VALUE2);
  EXPECT_EQ(s2->transition, TRANSITION2);

  struct gka_entry _s2;
  _s2.values.placement.start_time = START3;
  _s2.values.placement.value = VALUE3;
  _s2.transition = TRANSITION3;
  gka_local_address_t t3 = gka_extend_segment(m, t2, &_s2);

  struct gka_entry *s3 = gka_pointer(m, t3);
  EXPECT_EQ(s3->values.placement.start_time, START3);
  EXPECT_EQ(s3->values.placement.value, VALUE3);
  EXPECT_EQ(s3->transition, TRANSITION3);
}

TEST_F(GkaSegPatternFixture, PatternCreateTests) {
  struct gka_mem_block *m = gka_alloc_memblock(16 * sizeof(struct gka_entry));
  gka_local_address_t t = gka_pattern_create(m);
  struct gka_entry *p = gka_pointer(m, t);

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

  struct gka_entry _s;
  _s.values.placement.start_time = START2;
  _s.values.placement.value = VALUE2;
  _s.transition = TRANSITION2;
  gka_local_address_t t2 = gka_extend_segment(m, t, &_s);

  gka_segpattern_add_segment(m, p, &_s);
  struct gka_entry *s = gka_pointer(m, p->addr);
  EXPECT_EQ(s->values.placement.start_time, START2);
  EXPECT_EQ(s->values.placement.value, VALUE2);
  EXPECT_EQ(s->transition, TRANSITION2);

  _s.values.placement.start_time = START3;
  _s.values.placement.value = VALUE3;
  _s.transition = TRANSITION3;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START3);
  EXPECT_EQ(s->values.placement.value, VALUE3);
  EXPECT_EQ(s->transition, TRANSITION3);

  _s.values.placement.start_time = START4;
  _s.values.placement.value = VALUE4;
  _s.transition = TRANSITION4;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START4);
  EXPECT_EQ(s->values.placement.value, VALUE4);
  EXPECT_EQ(s->transition, TRANSITION4);
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

  struct gka_mem_block *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));
  gka_local_address_t t = gka_pattern_create(m);
  struct gka_entry *p = gka_pointer(m, t);

  struct gka_entry _s;
  _s.values.placement.start_time = START2;
  _s.values.placement.value = VALUE2;
  _s.transition = TRANSITION2;

  gka_segpattern_add_segment(m, p, &_s);
  struct gka_entry *s = gka_pointer(m, p->addr);
  EXPECT_EQ(s->values.placement.start_time, START2);
  EXPECT_EQ(s->values.placement.value, VALUE2);
  EXPECT_EQ(s->transition, TRANSITION2);

  _s.values.placement.start_time = START3;
  _s.values.placement.value = VALUE3;
  _s.transition = TRANSITION3;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START3);
  EXPECT_EQ(s->values.placement.value, VALUE3);
  EXPECT_EQ(s->transition, TRANSITION3);

  _s.values.placement.start_time = START4;
  _s.values.placement.value = VALUE4;
  _s.transition = TRANSITION4;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START4);
  EXPECT_EQ(s->values.placement.value, VALUE4);
  EXPECT_EQ(s->transition, TRANSITION4);

  // start adding a second pattern to the memblock
  gka_local_address_t p2t = gka_pattern_create(m);
  struct gka_entry *p2 = gka_pointer(m, p2t);

  _s.values.placement.start_time = P2_START;
  _s.values.placement.value = P2_VALUE;
  _s.transition = P2_TRANSITION;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, P2_START);
  EXPECT_EQ(s->values.placement.value, P2_VALUE);
  EXPECT_EQ(s->transition, P2_TRANSITION);

  _s.values.placement.start_time = P2_START2;
  _s.values.placement.value = P2_VALUE2;
  _s.transition = P2_TRANSITION2;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, P2_START2);
  EXPECT_EQ(s->values.placement.value, P2_VALUE2);
  EXPECT_EQ(s->transition, P2_TRANSITION2);

  _s.values.placement.start_time = P2_START3;
  _s.values.placement.value = P2_VALUE3;
  _s.transition = P2_TRANSITION3;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, P2_START3);
  EXPECT_EQ(s->values.placement.value, P2_VALUE3);
  EXPECT_EQ(s->transition, P2_TRANSITION3);

  _s.values.placement.start_time = P2_START4;
  _s.values.placement.value = P2_VALUE4;
  _s.transition = P2_TRANSITION4;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, P2_START4);
  EXPECT_EQ(s->values.placement.value, P2_VALUE4);
  EXPECT_EQ(s->transition, P2_TRANSITION4);

  // add more back from the original pattern now to cause a segment
  _s.values.placement.start_time = START5;
  _s.values.placement.value = VALUE5;
  _s.transition = TRANSITION5;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START5);
  EXPECT_EQ(s->values.placement.value, VALUE5);
  EXPECT_EQ(s->transition, TRANSITION5);

  _s.values.placement.start_time = START6;
  _s.values.placement.value = VALUE6;
  _s.transition = TRANSITION6;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START6);
  EXPECT_EQ(s->values.placement.value, VALUE6);
  EXPECT_EQ(s->transition, TRANSITION6);

  _s.values.placement.start_time = START7;
  _s.values.placement.value = VALUE7;
  _s.transition = TRANSITION7;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  EXPECT_EQ(s->values.placement.start_time, START7);
  EXPECT_EQ(s->values.placement.value, VALUE7);
  EXPECT_EQ(s->transition, TRANSITION7);

  test_print_mem_block(m);
}

TEST_F(GkaSegPatternFixture, SegmentFromSegmentTests) {
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

  struct gka_mem_block *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));
  gka_local_address_t t = gka_pattern_create(m);
  struct gka_entry *p = gka_pointer(m, t);

  struct gka_entry _s;
  _s.values.placement.start_time = START2;
  _s.values.placement.value = VALUE2;
  _s.transition = TRANSITION2;

  gka_segpattern_add_segment(m, p, &_s);
  struct gka_entry *s = gka_pointer(m, p->addr);

  _s.values.placement.start_time = START3;
  _s.values.placement.value = VALUE3;
  _s.transition = TRANSITION3;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = START4;
  _s.values.placement.value = VALUE4;
  _s.transition = TRANSITION4;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  // start adding a second pattern to the memblock
  gka_local_address_t p2t = gka_pattern_create(m);
  struct gka_entry *p2 = gka_pointer(m, p2t);

  _s.values.placement.start_time = P2_START;
  _s.values.placement.value = P2_VALUE;
  _s.transition = P2_TRANSITION;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = P2_START2;
  _s.values.placement.value = P2_VALUE2;
  _s.transition = P2_TRANSITION2;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = P2_START3;
  _s.values.placement.value = P2_VALUE3;
  _s.transition = P2_TRANSITION3;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = P2_START4;
  _s.values.placement.value = P2_VALUE4;
  _s.transition = P2_TRANSITION4;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  // add more back from the original pattern now to cause a segment
  _s.values.placement.start_time = START5;
  _s.values.placement.value = VALUE5;
  _s.transition = TRANSITION5;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = START6;
  _s.values.placement.value = VALUE6;
  _s.transition = TRANSITION6;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = START7;
  _s.values.placement.value = VALUE7;
  _s.transition = TRANSITION7;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);
  struct gka_entry *ep0_0 = gka_segment_from_pattern(m, p, 0);
  struct gka_entry *ep0_100 = gka_segment_from_pattern(m, p, 100);
  struct gka_entry *ep0_201 = gka_segment_from_pattern(m, p, 201);
  struct gka_entry *ep0_555 = gka_segment_from_pattern(m, p, 555);

  EXPECT_EQ(ep0_0->values.placement.value, 1.0);
  EXPECT_EQ(ep0_100->values.placement.value, 1.0);
  EXPECT_EQ(ep0_201->values.placement.value, 1.5);
  EXPECT_EQ(ep0_555->values.placement.value, 1.95);

  struct gka_entry *ep2_100 = gka_segment_from_pattern(m, p2, 100);
  struct gka_entry *ep2_375 = gka_segment_from_pattern(m, p2, 375);
  EXPECT_EQ(ep2_100->values.placement.value, 2.0);
  EXPECT_EQ(ep2_375->values.placement.value, 2.75);

  test_print_mem_block(m);
}

TEST_F(GkaSegPatternFixture, ValueFromSegmentTests) {
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

  struct gka_mem_block *m = gka_alloc_memblock(32 * sizeof(struct gka_entry));
  gka_local_address_t t = gka_pattern_create(m);
  struct gka_entry *p = gka_pointer(m, t);

  struct gka_entry _s;
  _s.values.placement.start_time = START2;
  _s.values.placement.value = VALUE2;
  _s.transition = TRANSITION2;

  gka_segpattern_add_segment(m, p, &_s);
  struct gka_entry *s = gka_pointer(m, p->addr);

  _s.values.placement.start_time = START3;
  _s.values.placement.value = VALUE3;
  _s.transition = TRANSITION3;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = START4;
  _s.values.placement.value = VALUE4;
  _s.transition = TRANSITION4;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  // start adding a second pattern to the memblock
  gka_local_address_t p2t = gka_pattern_create(m);
  struct gka_entry *p2 = gka_pointer(m, p2t);

  _s.values.placement.start_time = P2_START;
  _s.values.placement.value = P2_VALUE;
  _s.transition = P2_TRANSITION;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = P2_START2;
  _s.values.placement.value = P2_VALUE2;
  _s.transition = P2_TRANSITION2;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = P2_START3;
  _s.values.placement.value = P2_VALUE3;
  _s.transition = P2_TRANSITION3;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = P2_START4;
  _s.values.placement.value = P2_VALUE4;
  _s.transition = P2_TRANSITION4;
  t = gka_segpattern_add_segment(m, p2, &_s);

  s = gka_pointer(m, t);

  // add more back from the original pattern now to cause a segment
  _s.values.placement.start_time = START5;
  _s.values.placement.value = VALUE5;
  _s.transition = TRANSITION5;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = START6;
  _s.values.placement.value = VALUE6;
  _s.transition = TRANSITION6;
  t = gka_segpattern_add_segment(m, p, &_s);

  s = gka_pointer(m, t);

  _s.values.placement.start_time = START7;
  _s.values.placement.value = VALUE7;
  _s.transition = TRANSITION7;
  t = gka_segpattern_add_segment(m, p, &_s);

  test_print_mem_block(m);
  EXPECT_EQ(value_from_segment(m, p, 1.0, 0), 1.0);
  EXPECT_TRUE(FuzzyMatch(value_from_segment(m, p, 1.0, 150), 1.0, 1.5));
  EXPECT_TRUE(FuzzyMatch(value_from_segment(m, p, 1.0, 440), 1.85, 1.95));
  EXPECT_EQ(value_from_segment(m, p, 1.0, 1000), 1.99);

  EXPECT_EQ(value_from_segment(m, p2, 1.0, 0), 2.0);
}

} // namespace GekkotaTest

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}