#include "big.h"

#include <cstdint>

#include "gtest/gtest.h"

static int bign_compare_to(const struct bign *b, const char *ref) {
  if (b->data == nullptr) {
    return 4242;
  }

  struct bign bref;
  bign_create_from_string(&bref, ref);
  int cmp = bign_cmp(b, &bref);
  bign_destroy(&bref);
  return cmp;
}

static void bign_create_random(struct bign *self) {
  static const int SizeMin = 3;
  static const int SizeMax = 50;
  int size = SizeMin + std::rand() / (RAND_MAX / (SizeMax - SizeMin));
  assert(SizeMin <= size && size <= SizeMax);

  self->size = size;
  self->capacity = size;
  self->data = static_cast<uint32_t*>(std::calloc(size, sizeof(uint32_t)));

  for (int i = 0; i < size; ++i) {
    self->data[i] = (rand() << 16) + rand();
  }

  if (self->data[size - 1] == 0) {
    self->data[size - 1] = 1;
  }
}

static int bigz_compare_to(const struct bigz *b, const char *ref, unsigned base) {
  if (b->n.data == nullptr) {
    return 4242;
  }

  struct bigz bref;
  bigz_create_from_string(&bref, ref, base);
  int cmp = bigz_cmp(b, &bref);
  bigz_destroy(&bref);
  return cmp;
}

TEST(BigN, CreateEmpty) {
  struct bign b1;
  bign_create_empty(&b1);

  EXPECT_EQ(b1.size, 0u);
  EXPECT_EQ(b1.data, nullptr);

  bign_destroy(&b1);
}

TEST(BigN, CreateFromValue) {
  struct bign b1;
  bign_create_from_value(&b1, UINT32_C(42));

  EXPECT_EQ(b1.size, 1u);
  EXPECT_EQ(b1.data[0], UINT32_C(42));

  bign_destroy(&b1);
}

TEST(BigN, CreateFromStringZero) {
  struct bign b1;
  bign_create_from_string(&b1, "0");

  EXPECT_EQ(b1.size, 1u);
  EXPECT_EQ(b1.data[0], UINT32_C(0));

  bign_destroy(&b1);
}

TEST(BigN, CreateFromStringZeroes) {
  struct bign b1;
  bign_create_from_string(&b1, "00000000");

  EXPECT_EQ(b1.size, 1u);
  EXPECT_EQ(b1.data[0], UINT32_C(0));

  bign_destroy(&b1);
}

TEST(BigN, CreateFromStringDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "29A");

  EXPECT_EQ(b1.size, 1u);
  EXPECT_EQ(b1.data[0], UINT32_C(0x29A));

  bign_destroy(&b1);
}

TEST(BigN, CreateFromStringLong) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  EXPECT_EQ(b1.size, 6u);
  EXPECT_EQ(b1.data[5], UINT32_C(0xE38C3BA9));
  EXPECT_EQ(b1.data[4], UINT32_C(0x48CDEA56));
  EXPECT_EQ(b1.data[3], UINT32_C(0x73926BFF));
  EXPECT_EQ(b1.data[2], UINT32_C(0xDBAECBF7));
  EXPECT_EQ(b1.data[1], UINT32_C(0x82B3F2C3));
  EXPECT_EQ(b1.data[0], UINT32_C(0x32F1C3F2));

  bign_destroy(&b1);
}

TEST(BigN, CreateFromStringAllDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "0123456789ABCDEFabcdef");

  EXPECT_EQ(b1.size, 3u);
  EXPECT_EQ(b1.data[2], UINT32_C(0x012345));
  EXPECT_EQ(b1.data[1], UINT32_C(0x6789ABCD));
  EXPECT_EQ(b1.data[0], UINT32_C(0xEFABCDEF));

  bign_destroy(&b1);
}

TEST(BigN, CreateFromStringWithZero) {
  struct bign b1;
  bign_create_from_string(&b1, "0000000000000042");

  EXPECT_EQ(b1.size, 1u);
  EXPECT_EQ(b1.data[0], UINT32_C(0x42));

  bign_destroy(&b1);
}

TEST(BigN, CmpSame) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  int cmp = bign_cmp(&b1, &b1);
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b1);
}

TEST(BigN, CmpEqual) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_string(&b2, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  int cmp = bign_cmp(&b1, &b2);
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, CmpSameSizeGreater) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_string(&b2, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F1");

  int cmp = bign_cmp(&b1, &b2);
  EXPECT_GT(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, CmpSameSizeLower) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_string(&b2, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F3");

  int cmp = bign_cmp(&b1, &b2);
  EXPECT_LT(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, CmpDifferentSize) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_value(&b2, UINT32_C(42));

  {
    int cmp = bign_cmp(&b1, &b2);
    EXPECT_GT(cmp, 0);
  }

  {
    int cmp = bign_cmp(&b2, &b1);
    EXPECT_LT(cmp, 0);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, CmpToZero) {
  struct bign b1;
  bign_create_from_string(&b1, "0000000000000000");

  struct bign b2;
  bign_create_from_string(&b2, "0000000100000000");

  int cmp = bign_cmp(&b1, &b2);
  EXPECT_LT(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, CmpZeroEqual) {
  struct bign b1;
  bign_create_from_string(&b1, "0");

  int cmp = bign_cmp_zero(&b1);
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b1);
}

TEST(BigN, CmpZeroZeroEqual) {
  struct bign b1;
  bign_create_from_string(&b1, "0000000000000000");

  int cmp = bign_cmp_zero(&b1);
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b1);
}

TEST(BigN, CmpZeroDifferent) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  int cmp = bign_cmp_zero(&b1);
  EXPECT_GT(cmp, 0);

  bign_destroy(&b1);
}

TEST(BigN, Copy) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_empty(&b2);

  bign_copy_from_other(&b2, &b1);

  EXPECT_EQ(b2.size, 6u);

  if (b2.size == 6u) {
    EXPECT_EQ(b2.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(bign_cmp(&b1, &b2), 0);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, CopyInNonEmpty) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_string(&b2, "158EFA739BFA8362FBCD");

  bign_copy_from_other(&b2, &b1);

  EXPECT_EQ(b2.size, 6u);

  if (b2.size == 6u) {
    EXPECT_EQ(b2.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(bign_cmp(&b1, &b2), 0);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, Move) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  uint32_t *ptr = b1.data;

  struct bign b2;
  bign_create_empty(&b2);

  bign_move_from_other(&b2, &b1);

  EXPECT_EQ(b2.size, 6u);

  if (b2.size == 6u) {

    EXPECT_EQ(b2.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(ptr, b2.data);

    EXPECT_EQ(b1.size, 0u);
    EXPECT_EQ(b1.capacity, 0u);
    EXPECT_EQ(b1.data, nullptr);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MoveInNonEmpty) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  uint32_t *ptr = b1.data;

  struct bign b2;
  bign_create_from_string(&b2, "158EFA739BFA8362FBCD");

  bign_move_from_other(&b2, &b1);

  EXPECT_EQ(b2.size, 6u);

  if (b2.size == 6u) {

    EXPECT_EQ(b2.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(ptr, b2.data);

    EXPECT_EQ(b1.size, 0u);
    EXPECT_EQ(b1.capacity, 0u);
    EXPECT_EQ(b1.data, nullptr);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, Print) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  bign_print(&b1); puts("");

  bign_destroy(&b1);
}

TEST(BigN, PrintWithZero){ //rajouté
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA56739000000002C332F1C3F2");
  bign_print(&b1);
  puts("");
  bign_destroy(&b1);

}

/*
 * add
 */

TEST(BigN, AddSingleDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "12");

  struct bign b2;
  bign_create_from_string(&b2, "34");

  struct bign b3;
  bign_create_empty(&b3);

  bign_add(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "46");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, AddSingleDigitToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "12");

  struct bign b2;
  bign_create_from_string(&b2, "34");

  bign_add(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "46");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, AddManyDigits) {
  struct bign b1;
  bign_create_from_string(&b1, "1212121212121212");

  struct bign b2;
  bign_create_from_string(&b2, "3434343434343434");

  struct bign b3;
  bign_create_empty(&b3);

  bign_add(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "4646464646464646");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, AddManyDigitsToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "1212121212121212");

  struct bign b2;
  bign_create_from_string(&b2, "3434343434343434");

  bign_add(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "4646464646464646");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, AddManyDigitsWithCarry) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "1");

  struct bign b3;
  bign_create_empty(&b3);

  bign_add(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "10000000000000000");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, AddManyDigitsWithCarryToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "1");

  bign_add(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "10000000000000000");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, AddMax) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "FFFFFFFFFFFFFFFF");

  struct bign b3;
  bign_create_empty(&b3);

  bign_add(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1fffffffffffffffe");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

/*
 * sub
 */

TEST(BigN, SubSingleDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "46");

  struct bign b2;
  bign_create_from_string(&b2, "34");

  struct bign b3;
  bign_create_empty(&b3);

  bign_sub(&b3, &b1, &b2);

  bign_print(&b1); puts("");
  bign_print(&b2); puts("");
  bign_print(&b3); puts("");

  int cmp = bign_compare_to(&b3, "12");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, SubSingleDigitToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "46");

  struct bign b2;
  bign_create_from_string(&b2, "34");

  bign_sub(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "12");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, SubManyDigits) {
  struct bign b1;
  bign_create_from_string(&b1, "4646464646464646");

  struct bign b2;
  bign_create_from_string(&b2, "3434343434343434");

  struct bign b3;
  bign_create_empty(&b3);

  bign_sub(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1212121212121212");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, SubManyDigitsToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "4646464646464646");

  struct bign b2;
  bign_create_from_string(&b2, "3434343434343434");

  bign_sub(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "1212121212121212");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, SubManyDigitsWithCarry) {
  struct bign b1;
  bign_create_from_string(&b1, "10000000000000000");

  struct bign b2;
  bign_create_from_string(&b2, "1");

  struct bign b3;
  bign_create_empty(&b3);

  bign_sub(&b3, &b1, &b2);

  bign_print(&b1); puts("");
  bign_print(&b2); puts("");
  bign_print(&b3); puts("");

  int cmp = bign_compare_to(&b3, "FFFFFFFFFFFFFFFF");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, SubManyDigitsWithCarryToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "10000000000000000");

  struct bign b2;
  bign_create_from_string(&b2, "1");

  bign_sub(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "FFFFFFFFFFFFFFFF");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, SubMax) {
  struct bign b1;
  bign_create_from_string(&b1, "10000000000000000");

  struct bign b2;
  bign_create_from_string(&b2, "FFFFFFFFFFFFFFFF");

  struct bign b3;
  bign_create_empty(&b3);

  bign_sub(&b3, &b1, &b2);

  bign_print(&b1); puts("");
  bign_print(&b2); puts("");
  bign_print(&b3); puts("");

  int cmp = bign_compare_to(&b3, "1");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}


// TODO: X - 0

/*
 * mul
 */

TEST(BigN, MulSingleDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "42");

  struct bign b2;
  bign_create_from_string(&b2, "69");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1b12");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulSingleDigitToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "42");

  struct bign b2;
  bign_create_from_string(&b2, "69");

  bign_mul(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "1b12");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}


TEST(BigN, MulManyDigits) {
  struct bign b1;
  bign_create_from_string(&b1, "69426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "12");

  {
    struct bign b3;
    bign_create_empty(&b3);
    bign_mul(&b3, &b1, &b2);
    int cmp = bign_compare_to(&b3, "766ab66ab66ab62");
    EXPECT_EQ(cmp, 0);
    bign_destroy(&b3);
  }

  {
    struct bign b3;
    bign_create_empty(&b3);
    bign_mul(&b3, &b2, &b1);
    int cmp = bign_compare_to(&b3, "766ab66ab66ab62");
    EXPECT_EQ(cmp, 0);
    bign_destroy(&b3);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulManyDigitsToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "69426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "12");

  bign_mul(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "766ab66ab66ab62");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}


TEST(BigN, MulManyDigitsWithCarry) {
  struct bign b1;
  bign_create_from_string(&b1, "4269426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "1200000012");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "4ab66ab6b56cd56c8ab66ab62");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulManyDigitsWithCarryToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "4269426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "1200000012");

  bign_mul(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "4ab66ab6b56cd56c8ab66ab62");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulMax) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "FFFFFFFFFFFFFFFF");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "fffffffffffffffe0000000000000001");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulRsa768) {
  struct bign b1;
  bign_create_from_string(&b1, "d982ec7b440e2869d2535e51f91bacc3eb6eba042e106e6f875c3d17e53db65fffd6e4e9a36084ce60f83d754dd7f701");

  struct bign b2;
  bign_create_from_string(&b2, "eebe6dd23ce7e99c0e2249fecc4418c34af74e418bfa714c3791828414ab18f32fd7e093062a49b030225cc845f99ab5");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "cad984557c97e039431a226ad727f0c6d43ef3d418469f1b375049b229843ee9f83b1f97738ac274f5f61f401f21f1913e4b64bb31b55a38d398c0dfed00b1392f0889711c44b359e7976c617fcc734f06e3e95c26476091b52f462e79413db5");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

/*
 * mul short TODO
 */


/*
 * mul karatsuba
 */

TEST(BigN, MulKaratsubaSingleDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "42");

  struct bign b2;
  bign_create_from_string(&b2, "69");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul_karatsuba(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1b12");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulKaratsubaSingleDigitToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "42");

  struct bign b2;
  bign_create_from_string(&b2, "69");

  bign_mul_karatsuba(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "1b12");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}


TEST(BigN, MulKaratsubaManyDigits) {
  struct bign b1;
  bign_create_from_string(&b1, "69426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "12");

  {
    struct bign b3;
    bign_create_empty(&b3);
    bign_mul_karatsuba(&b3, &b1, &b2);
    int cmp = bign_compare_to(&b3, "766ab66ab66ab62");
    EXPECT_EQ(cmp, 0);
    bign_destroy(&b3);
  }

  {
    struct bign b3;
    bign_create_empty(&b3);
    bign_mul_karatsuba(&b3, &b2, &b1);
    int cmp = bign_compare_to(&b3, "766ab66ab66ab62");
    EXPECT_EQ(cmp, 0);
    bign_destroy(&b3);
  }

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulKaratsubaManyDigitsToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "69426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "12");

  bign_mul_karatsuba(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "766ab66ab66ab62");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}


TEST(BigN, MulKaratsubaManyDigitsWithCarry) {
  struct bign b1;
  bign_create_from_string(&b1, "4269426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "1200000012");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul_karatsuba(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "4ab66ab6b56cd56c8ab66ab62");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulKaratsubaManyDigitsWithCarryToSelf) {
  struct bign b1;
  bign_create_from_string(&b1, "4269426942694269");

  struct bign b2;
  bign_create_from_string(&b2, "1200000012");

  bign_mul_karatsuba(&b1, &b1, &b2);

  int cmp = bign_compare_to(&b1, "4ab66ab6b56cd56c8ab66ab62");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulKaratsubaMax) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "FFFFFFFFFFFFFFFF");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul_karatsuba(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "fffffffffffffffe0000000000000001");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, MulKaratsubaRsa768) {
  struct bign b1;
  bign_create_from_string(&b1, "d982ec7b440e2869d2535e51f91bacc3eb6eba042e106e6f875c3d17e53db65fffd6e4e9a36084ce60f83d754dd7f701");

  struct bign b2;
  bign_create_from_string(&b2, "eebe6dd23ce7e99c0e2249fecc4418c34af74e418bfa714c3791828414ab18f32fd7e093062a49b030225cc845f99ab5");

  struct bign b3;
  bign_create_empty(&b3);

  bign_mul_karatsuba(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "cad984557c97e039431a226ad727f0c6d43ef3d418469f1b375049b229843ee9f83b1f97738ac274f5f61f401f21f1913e4b64bb31b55a38d398c0dfed00b1392f0889711c44b359e7976c617fcc734f06e3e95c26476091b52f462e79413db5");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

/*
 * div
 */

TEST(BigN, DivSingleDigit) {
  struct bign b1;
  bign_create_from_string(&b1, "69");

  struct bign b2;
  bign_create_from_string(&b2, "42");

  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b1, &b2);

  int cmpq = bign_compare_to(&q, "1");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "27");
  EXPECT_EQ(cmpr, 0);

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivSingleDigitReverse) {
  struct bign b1;
  bign_create_from_string(&b1, "69");

  struct bign b2;
  bign_create_from_string(&b2, "42");

  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b2, &b1);

  int cmpq = bign_compare_to(&q, "0");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "42");
  EXPECT_EQ(cmpr, 0);

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivShort) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_string(&b2, "4269");

  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b1, &b2);

  int cmpq = bign_compare_to(&q, "36d289377777bc42aa33c0180efb449c6ca5908b4dc2c");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "1de6");
  EXPECT_EQ(cmpr, 0);

  struct bign b3;
  bign_create_from_string(&b3, "36d289377777bc42aa33c0180efb449c6ca5908b4dc2c");

  for(size_t i = 0; i < b3.size; ++i){
    printf("quo data %lu = %u\n",i,b3.data[i]);
  }

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivShortMax) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "FFFFFFFF");

  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b1, &b2);

  int cmpq = bign_compare_to(&q, "100000001");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "0");
  EXPECT_EQ(cmpr, 0);

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivManyDigits) {
  struct bign b1;
  bign_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2");

  struct bign b2;
  bign_create_from_string(&b2, "426999FEAB350EB291B3F1098");

  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b1, &b2);

  int cmpq = bign_compare_to(&q, "36d20a18b7884e2890017831");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "222ada30e45d62ecadb8056da");
  EXPECT_EQ(cmpr, 0);

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivMax) {
  struct bign b1;
  bign_create_from_string(&b1, "FFFFFFFFFFFFFFFF");

  struct bign b2;
  bign_create_from_string(&b2, "FFFFFFFFFFFFFFFF");

  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b1, &b2);

  int cmpq = bign_compare_to(&q, "1");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "0");
  EXPECT_EQ(cmpr, 0);

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}


/*
 * tests from http://www.hackersdelight.org/hdcodetxt/divmnu64.c.txt
 * the first three tests are errors
 */

TEST(BigN, DivCase3) {
  // test #3
  struct bign b1;
  bign_create_from_string(&b1, "00000003");
  struct bign b2;
  bign_create_from_string(&b2, "00000002");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000001");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase4) {
  // test #4
  struct bign b1;
  bign_create_from_string(&b1, "00000003");
  struct bign b2;
  bign_create_from_string(&b2, "00000003");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase5) {
  // test #5
  struct bign b1;
  bign_create_from_string(&b1, "00000003");
  struct bign b2;
  bign_create_from_string(&b2, "00000004");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000003");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase6) {
  // test #6
  struct bign b1;
  bign_create_from_string(&b1, "00000000");
  struct bign b2;
  bign_create_from_string(&b2, "ffffffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase7) {
  // test #7
  struct bign b1;
  bign_create_from_string(&b1, "ffffffff");
  struct bign b2;
  bign_create_from_string(&b2, "00000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "ffffffff");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase8) {
  // test #8
  struct bign b1;
  bign_create_from_string(&b1, "ffffffff");
  struct bign b2;
  bign_create_from_string(&b2, "ffffffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase9) {
  // test #9
  struct bign b1;
  bign_create_from_string(&b1, "ffffffff");
  struct bign b2;
  bign_create_from_string(&b2, "00000003");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "55555555");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase10) {
  // test #10
  struct bign b1;
  bign_create_from_string(&b1, "ffffffffffffffff");
  struct bign b2;
  bign_create_from_string(&b2, "00000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "ffffffffffffffff");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase11) {
  // test #11
  struct bign b1;
  bign_create_from_string(&b1, "ffffffffffffffff");
  struct bign b2;
  bign_create_from_string(&b2, "ffffffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "0000000100000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase12) {
  // test #12
  struct bign b1;
  bign_create_from_string(&b1, "fffffffeffffffff");
  struct bign b2;
  bign_create_from_string(&b2, "ffffffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000ffffffff");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "fffffffe");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase13) {
  // test #13
  struct bign b1;
  bign_create_from_string(&b1, "0000123400005678");
  struct bign b2;
  bign_create_from_string(&b2, "00009abc");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "000000001e1dba76");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00006bd0");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase14) {
  // test #14
  struct bign b1;
  bign_create_from_string(&b1, "0000000000000000");
  struct bign b2;
  bign_create_from_string(&b2, "0000000100000000");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000000000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase15) {
  // test #15
  struct bign b1;
  bign_create_from_string(&b1, "0000000700000000");
  struct bign b2;
  bign_create_from_string(&b2, "0000000300000000");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000002");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000100000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase16) {
  // test #16
  struct bign b1;
  bign_create_from_string(&b1, "0000000700000005");
  struct bign b2;
  bign_create_from_string(&b2, "0000000300000000");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000002");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000100000005");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase17) {
  // test #17
  struct bign b1;
  bign_create_from_string(&b1, "0000000600000000");
  struct bign b2;
  bign_create_from_string(&b2, "0000000200000000");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000003");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000000000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase18) {
  // test #18
  struct bign b1;
  bign_create_from_string(&b1, "80000000");
  struct bign b2;
  bign_create_from_string(&b2, "40000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "3fffffff");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase19) {
  // test #19
  struct bign b1;
  bign_create_from_string(&b1, "8000000000000000");
  struct bign b2;
  bign_create_from_string(&b2, "40000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001fffffff8");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000008");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase20) {
  // test #20
  struct bign b1;
  bign_create_from_string(&b1, "8000000000000000");
  struct bign b2;
  bign_create_from_string(&b2, "4000000000000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "3fffffffffffffff");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase21) {
  // test #21
  struct bign b1;
  bign_create_from_string(&b1, "0000bcde0000789a");
  struct bign b2;
  bign_create_from_string(&b2, "0000bcde0000789a");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000000000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase22) {
  // test #22
  struct bign b1;
  bign_create_from_string(&b1, "0000bcde0000789b");
  struct bign b2;
  bign_create_from_string(&b2, "0000bcde0000789a");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000000000001");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase23) {
  // test #23
  struct bign b1;
  bign_create_from_string(&b1, "0000bcde00007899");
  struct bign b2;
  bign_create_from_string(&b2, "0000bcde0000789a");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000bcde00007899");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase24) {
  // test #24
  struct bign b1;
  bign_create_from_string(&b1, "0000ffff0000ffff");
  struct bign b2;
  bign_create_from_string(&b2, "0000ffff0000ffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000001");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "0000000000000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase25) {
  // test #25
  struct bign b1;
  bign_create_from_string(&b1, "0000ffff0000ffff");
  struct bign b2;
  bign_create_from_string(&b2, "0000000100000000");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "0000ffff");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "000000000000ffff");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase26) {
  // test #26
  struct bign b1;
  bign_create_from_string(&b1, "0000012300004567000089ab");
  struct bign b2;
  bign_create_from_string(&b2, "0000000100000000");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "0000012300004567");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000000089ab");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase27) {
  // test #27
  // Shows that first qhat can = b + 1.
  struct bign b1;
  bign_create_from_string(&b1, "000080000000fffe00000000");

  struct bign b2;
  bign_create_from_string(&b2, "000080000000ffff");

  struct bign q;
  bign_create_empty(&q);

  struct bign r;
  bign_create_empty(&r);

  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000ffffffff");
  EXPECT_EQ(cmpq, 0);

  int cmpr = bign_compare_to(&r, "00007fff0000ffff");
  EXPECT_EQ(cmpr, 0);

  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase28) {
  // test #28
  // Adding back step req'd.
  struct bign b1;
  bign_create_from_string(&b1, "800000000000000000000003");
  struct bign b2;
  bign_create_from_string(&b2, "200000000000000000000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000003");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "200000000000000000000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase29) {
  // test #29
  // Adding back step req'd.
  struct bign b1;
  bign_create_from_string(&b1, "000080000000000000000003");
  struct bign b2;
  bign_create_from_string(&b2, "000020000000000000000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000003");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "000020000000000000000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase30) {
  // test #30
  // Add back req'd.
  struct bign b1;
  bign_create_from_string(&b1, "00007fff000080000000000000000000");
  struct bign b2;
  bign_create_from_string(&b2, "000080000000000000000001");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000fffe0000");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00007fffffffffff00020000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase31) {
  // test #31
  // Shows that mult-sub quantity cannot be treated as signed.
  struct bign b1;
  bign_create_from_string(&b1, "00008000000000000000fffe00000000");
  struct bign b2;
  bign_create_from_string(&b2, "00008000000000000000ffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000ffffffff");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00007fffffffffff0000ffff");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase32) {
  // test #32
  // Shows that mult-sub quantity cannot be treated as signed.
  struct bign b1;
  bign_create_from_string(&b1, "8000000000000000fffffffe00000000");
  struct bign b2;
  bign_create_from_string(&b2, "80000000000000000000ffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "0000000100000000");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "00000000fffeffff00000000");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, DivCase33) {
  // test #33
  // Shows that mult-sub quantity cannot be treated as signed.
  struct bign b1;
  bign_create_from_string(&b1, "8000000000000000fffffffe00000000");
  struct bign b2;
  bign_create_from_string(&b2, "8000000000000000ffffffff");
  struct bign q;
  bign_create_empty(&q);
  struct bign r;
  bign_create_empty(&r);
  bign_div(&q, &r, &b1, &b2);
  int cmpq = bign_compare_to(&q, "00000000ffffffff");
  EXPECT_EQ(cmpq, 0);
  int cmpr = bign_compare_to(&r, "7fffffffffffffffffffffff");
  EXPECT_EQ(cmpr, 0);
  bign_destroy(&r);
  bign_destroy(&q);
  bign_destroy(&b2);
  bign_destroy(&b1);
}


/*
 * exp
 */

// exp( , 0)

TEST(BigN, ExpXPowerZero) {
  struct bign b1;
  bign_create_from_string(&b1, "1234567890ABCDEF");

  struct bign b2;
  bign_create_from_string(&b2, "0");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

// exp( , 1)

TEST(BigN, ExpXPowerOne) {
  struct bign b1;
  bign_create_from_string(&b1, "1234567890ABCDEF");

  struct bign b2;
  bign_create_from_string(&b2, "1");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1234567890ABCDEF");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

// exp(0,  )

TEST(BigN, ExpZeroPowerN) {
  struct bign b1;
  bign_create_from_string(&b1, "0");

  struct bign b2;
  bign_create_from_string(&b2, "1234567890ABCDEF");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "0");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

// exp(1,  )

TEST(BigN, ExpOnePowerN) {
  struct bign b1;
  bign_create_from_string(&b1, "1");

  struct bign b2;
  bign_create_from_string(&b2, "1234567890ABCDEF");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "1");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

// exp(sm, sm)

TEST(BigN, ExpSmallPowerSmall) {
  struct bign b1;
  bign_create_from_string(&b1, "31");

  struct bign b2;
  bign_create_from_string(&b2, "56");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "7491caf9dd4e6cc7aa542df60f1437aad480c03e54f97e58230a7952097d3710e2c0af4562b67af9f8e46b8c40beaa50fadf46423471861e4efce4f21");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

// exp(lg, sm)

TEST(BigN, ExpLargePowerSmall) {
  struct bign b1;
  bign_create_from_string(&b1, "123456789");

  struct bign b2;
  bign_create_from_string(&b2, "13");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  int cmp = bign_compare_to(&b3, "b9def572bcd526c6bc46607dde0446ad6da1f4c4302fd57a1bec55d6157b328aeabe4525706f87494c647dca5fd70e1c33bbc93a4291fcf0eb957d7262a84b96148baee40ed8836bbeef433d9");
  EXPECT_EQ(cmp, 0);

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

// exp(sm, lg)

TEST(BigN, ExpSmallPowerLarge) {
  struct bign b1;
  bign_create_from_string(&b1, "2");

  struct bign b2;
  bign_create_from_string(&b2, "1000");

  struct bign b3;
  bign_create_empty(&b3);

  bign_exp(&b3, &b1, &b2);

  EXPECT_EQ(b3.size, 129u);

  if (b3.size == 129u) {
    EXPECT_EQ(b3.data[128], UINT32_C(1));

    for (int i = 0; i < 128; ++i) {
      EXPECT_EQ(b3.data[i], UINT32_C(0));
    }
  }

  bign_destroy(&b3);
  bign_destroy(&b2);
  bign_destroy(&b1);
}

TEST(BigN, RandomAddSub) {
  static const int Iterations = 1000;

  for (int i = 0; i < Iterations; ++i) {
    struct bign b1;
    bign_create_random(&b1);

    struct bign b2;
    bign_create_random(&b2);

    struct bign b3;
    bign_create_empty(&b3);

    bign_add(&b3, &b1, &b2);

    struct bign b4;
    bign_create_empty(&b4);

    bign_sub(&b4, &b3, &b2);

    int cmp = bign_cmp(&b4, &b1);

    bign_destroy(&b4);
    bign_destroy(&b3);
    bign_destroy(&b2);
    bign_destroy(&b1);

    ASSERT_EQ(cmp, 0);
  }

}

TEST(BigN, RandomMulMul) {
  static const int Iterations = 100;

  for (int i = 0; i < Iterations; ++i) {
    struct bign b1;
    bign_create_random(&b1);

    struct bign b2;
    bign_create_random(&b2);

    struct bign b3;
    bign_create_empty(&b3);

    bign_mul(&b3, &b1, &b2);

    struct bign b4;
    bign_create_empty(&b4);

    bign_mul_karatsuba(&b4, &b1, &b2);

    int cmp = bign_cmp(&b3, &b4);

    bign_destroy(&b4);
    bign_destroy(&b3);
    bign_destroy(&b2);
    bign_destroy(&b1);

    ASSERT_EQ(cmp, 0);
  }

}

TEST(BigN, RandomMulDiv) {
  static const int Iterations = 1000;

  for (int i = 0; i < Iterations; ++i) {
    struct bign b1;
    bign_create_random(&b1);

    struct bign b2;
    bign_create_random(&b2);

    struct bign quo;
    bign_create_empty(&quo);

    struct bign rem;
    bign_create_empty(&rem);

    bign_div(&quo, &rem, &b1, &b2);

    struct bign b3;
    bign_create_random(&b3);

    bign_mul(&b3, &quo, &b2);
    bign_add(&b3, &b3, &rem);

    int cmp = bign_cmp(&b3, &b1);

    bign_destroy(&b3);
    bign_destroy(&rem);
    bign_destroy(&quo);
    bign_destroy(&b2);
    bign_destroy(&b1);

    ASSERT_EQ(cmp, 0);
  }

}

/*
 * BigZ
 */

TEST(BigZ, CreateEmpty) {
  struct bigz b1;
  bigz_create_empty(&b1);

  EXPECT_EQ(b1.n.size, 0u);
  EXPECT_EQ(b1.n.data, nullptr);

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromValuePositive) {
  struct bigz b1;
  bigz_create_from_value(&b1, INT32_C(42));

  EXPECT_TRUE(b1.positive);
  EXPECT_EQ(b1.n.size, 1u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(42));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromValueNegative) {
  struct bigz b1;
  bigz_create_from_value(&b1, INT32_C(-42));

  EXPECT_FALSE(b1.positive);
  EXPECT_EQ(b1.n.size, 1u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(42));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromValuePositiveMax) {
  struct bigz b1;
  bigz_create_from_value(&b1, INT32_MAX);

  uint32_t ref = INT32_MAX;

  EXPECT_TRUE(b1.positive);
  EXPECT_EQ(b1.n.size, 1u);
  EXPECT_EQ(b1.n.data[0], ref);

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromValueNegativeMin) {
  struct bigz b1;
  bigz_create_from_value(&b1, INT32_MIN);

  EXPECT_FALSE(b1.positive);
  EXPECT_EQ(b1.n.size, 1u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(2147483648));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromStringPositiveZero) {
  struct bigz b1;
  bigz_create_from_string(&b1, "0", 10);

  EXPECT_TRUE(b1.positive);
  EXPECT_EQ(b1.n.size, 1u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(0));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromStringNegativeZero) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-0", 10);

  EXPECT_FALSE(b1.positive);
  EXPECT_EQ(b1.n.size, 1u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(0));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromStringBinary) {
  struct bigz b1;
  bigz_create_from_string(&b1, "1001101101111010010010101101101011110110011100010001110111010001010001011110100010101010", 2);

  EXPECT_TRUE(b1.positive);
  EXPECT_EQ(b1.n.size, 3u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(0xd145e8aa));
  EXPECT_EQ(b1.n.data[1], UINT32_C(0xdaf6711d));
  EXPECT_EQ(b1.n.data[2], UINT32_C(0x9b7a4a));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromStringDecimal) {
  struct bigz b1;
  bigz_create_from_string(&b1, "15497642151661527013020516797518124587412201", 10);

  EXPECT_TRUE(b1.positive);
  EXPECT_EQ(b1.n.size, 5u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(0x1507eae9));
  EXPECT_EQ(b1.n.data[1], UINT32_C(0x7698c6e5));
  EXPECT_EQ(b1.n.data[2], UINT32_C(0x57589b7b));
  EXPECT_EQ(b1.n.data[3], UINT32_C(0x7a1cba70));
  EXPECT_EQ(b1.n.data[4], UINT32_C(0xb1e7));

  bigz_destroy(&b1);
}

TEST(BigZ, CreateFromStringDecimalNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-15497642151661527013020516797518124587412201", 10);

  EXPECT_FALSE(b1.positive);
  EXPECT_EQ(b1.n.size, 5u);
  EXPECT_EQ(b1.n.data[0], UINT32_C(0x1507eae9));
  EXPECT_EQ(b1.n.data[1], UINT32_C(0x7698c6e5));
  EXPECT_EQ(b1.n.data[2], UINT32_C(0x57589b7b));
  EXPECT_EQ(b1.n.data[3], UINT32_C(0x7a1cba70));
  EXPECT_EQ(b1.n.data[4], UINT32_C(0xb1e7));

  bigz_destroy(&b1);
}

TEST(BigZ, CmpSame) {
  struct bigz b1;
  bigz_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  int cmp = bigz_cmp(&b1, &b1);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b1);
}

TEST(BigZ, CmpEqualPositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  struct bigz b2;
  bigz_create_from_string(&b2, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  int cmp = bigz_cmp(&b1, &b2);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, CmpEqualNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  struct bigz b2;
  bigz_create_from_string(&b2, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  int cmp = bigz_cmp(&b1, &b2);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, CmpDifferentSign) {
  struct bigz b1;
  bigz_create_from_string(&b1, "E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  struct bigz b2;
  bigz_create_from_string(&b2, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  {
    int cmp = bigz_cmp(&b1, &b2);
    EXPECT_GT(cmp, 0);
  }

  {
    int cmp = bigz_cmp(&b2, &b1);
    EXPECT_LT(cmp, 0);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

// TODO: negative negative

TEST(BigZ, CmpPositiveNegativeZero) {
  struct bigz b1;
  bigz_create_from_string(&b1, "0", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-0", 10);

  {
    int cmp = bigz_cmp(&b1, &b2);
    EXPECT_EQ(cmp, 0);
  }

  {
    int cmp = bigz_cmp(&b2, &b1);
    EXPECT_EQ(cmp, 0);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, CmpZeroEqualPositiveZero) {
  struct bigz b1;
  bigz_create_from_string(&b1, "0", 10);

  int cmp = bigz_cmp_zero(&b1);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b1);
}

TEST(BigZ, CmpZeroEqualNegativeZero) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-0", 10);

  int cmp = bigz_cmp_zero(&b1);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b1);
}

TEST(BigZ, CmpZeroPositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "12345678987654321", 10);

  int cmp = bigz_cmp_zero(&b1);
  EXPECT_GT(cmp, 0);

  bigz_destroy(&b1);
}

TEST(BigZ, CmpZeroNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-12345678987654321", 10);

  int cmp = bigz_cmp_zero(&b1);
  EXPECT_LT(cmp, 0);

  bigz_destroy(&b1);
}


TEST(BigZ, Copy) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  struct bigz b2;
  bigz_create_empty(&b2);

  bigz_copy_from_other(&b2, &b1);

  EXPECT_FALSE(b2.positive);
  EXPECT_EQ(b2.n.size, 6u);

  if (b2.n.size == 6u) {
    EXPECT_EQ(b2.n.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.n.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.n.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.n.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.n.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.n.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(bigz_cmp(&b1, &b2), 0);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, CopyInNonEmpty) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  struct bigz b2;
  bigz_create_from_string(&b2, "EF193C2DFAC33C2FF3D89274801", 16);

  bigz_copy_from_other(&b2, &b1);

  EXPECT_FALSE(b2.positive);
  EXPECT_EQ(b2.n.size, 6u);

  if (b2.n.size == 6u) {
    EXPECT_EQ(b2.n.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.n.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.n.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.n.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.n.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.n.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(bigz_cmp(&b1, &b2), 0);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, Move) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  uint32_t *ptr = b1.n.data;

  struct bigz b2;
  bigz_create_empty(&b2);

  bigz_move_from_other(&b2, &b1);

  EXPECT_FALSE(b2.positive);
  EXPECT_EQ(b2.n.size, 6u);

  if (b2.n.size == 6u) {
    EXPECT_EQ(b2.n.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.n.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.n.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.n.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.n.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.n.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(ptr, b2.n.data);

    EXPECT_EQ(b1.n.size, 0u);
    EXPECT_EQ(b1.n.capacity, 0u);
    EXPECT_EQ(b1.n.data, nullptr);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, MoveInNonEmpty) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);

  uint32_t *ptr = b1.n.data;

  struct bigz b2;
  bigz_create_from_string(&b2, "EF193C2DFAC33C2FF3D89274801", 16);

  bigz_move_from_other(&b2, &b1);

  EXPECT_FALSE(b2.positive);
  EXPECT_EQ(b2.n.size, 6u);

  if (b2.n.size == 6u) {
    EXPECT_EQ(b2.n.data[5], UINT32_C(0xE38C3BA9));
    EXPECT_EQ(b2.n.data[4], UINT32_C(0x48CDEA56));
    EXPECT_EQ(b2.n.data[3], UINT32_C(0x73926BFF));
    EXPECT_EQ(b2.n.data[2], UINT32_C(0xDBAECBF7));
    EXPECT_EQ(b2.n.data[1], UINT32_C(0x82B3F2C3));
    EXPECT_EQ(b2.n.data[0], UINT32_C(0x32F1C3F2));

    EXPECT_EQ(ptr, b2.n.data);

    EXPECT_EQ(b1.n.size, 0u);
    EXPECT_EQ(b1.n.capacity, 0u);
    EXPECT_EQ(b1.n.data, nullptr);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, Print) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-E38C3BA948CDEA5673926BFFDBAECBF782B3F2C332F1C3F2", 16);
  bigz_print(&b1); puts("");

  bigz_destroy(&b1);
}

TEST(BigZ, AddPositivePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "46578458745802102102145458458245485441416", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, AddPositiveNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-46578458745802102102145458458245485441416", 10);


  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, AddNegativePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "46578458745802102102145458458245485441416", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "-231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "-231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, AddNegativeNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-46578458745802102102145458458245485441416", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "-324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_add(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "-324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}




TEST(BigZ, SubPositivePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "46578458745802102102145458458245485441416", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "-231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, SubPositiveNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-46578458745802102102145458458245485441416", 10);


  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "-324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, SubNegativePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "46578458745802102102145458458245485441416", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "-324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "324808583067597272281203376099393387231117", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, SubNegativeNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-46578458745802102102145458458245485441416", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "-231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_sub(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "231651665575993068076912459182902416348285", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}


TEST(BigZ, MulPositivePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "4657845874580210210", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, MulPositiveNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-4657845874580210210", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "-1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "-1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, MulNegativePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "4657845874580210210", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "-1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "-1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, MulNegativeNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-4657845874580210210", 10);

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b1, &b2);

    int cmp = bigz_compare_to(&b3, "1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  {
    struct bigz b3;
    bigz_create_empty(&b3);

    bigz_mul(&b3, &b2, &b1);

    int cmp = bigz_compare_to(&b3, "1295953036756212640552727646158429498971649147282135793047210", 10);
    EXPECT_EQ(cmp, 0);

    bigz_destroy(&b3);
  }

  bigz_destroy(&b2);
  bigz_destroy(&b1);
}


TEST(BigZ, DivPositivePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  int cmpq = bigz_compare_to(&q, "59733647658934344963858", 10);
  EXPECT_EQ(cmpq, 0);

  int cmpr = bigz_compare_to(&r, "2948551563409199521", 10);
  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, DivPositiveNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  int cmpq = bigz_compare_to(&q, "-59733647658934344963858", 10);
  EXPECT_EQ(cmpq, 0);

  int cmpr = bigz_compare_to(&r, "2948551563409199521", 10);
  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, DivNegativePositive) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  // int cmpq = bigz_compare_to(&q, "-59733647658934344963858", 10);
  int cmpq = bigz_compare_to(&q, "-59733647658934344963859", 10);
  EXPECT_EQ(cmpq, 0);

  // int cmpr = bigz_compare_to(&r, "-2948551563409199521", 10);
  int cmpr = bigz_compare_to(&r, "1709294311171010689", 10);

  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, DivNegativeNegative) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179057917641147901789701", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  // int cmpq = bigz_compare_to(&q, "59733647658934344963858", 10);
  int cmpq = bigz_compare_to(&q, "59733647658934344963859", 10);
  EXPECT_EQ(cmpq, 0);

  // int cmpr = bigz_compare_to(&r, "-2948551563409199521", 10);
  int cmpr = bigz_compare_to(&r, "1709294311171010689", 10);
  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}



TEST(BigZ, DivPositivePositiveZeroRemainder) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179054969089584492590180", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  int cmpq = bigz_compare_to(&q, "59733647658934344963858", 10);
  EXPECT_EQ(cmpq, 0);

  int cmpr = bigz_compare_to(&r, "0", 10);
  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, DivPositiveNegativeZeroRemainder) {
  struct bigz b1;
  bigz_create_from_string(&b1, "278230124321795170179054969089584492590180", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  int cmpq = bigz_compare_to(&q, "-59733647658934344963858", 10);
  EXPECT_EQ(cmpq, 0);

  int cmpr = bigz_compare_to(&r, "0", 10);
  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, DivNegativePositiveZeroRemainder) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179054969089584492590180", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  int cmpq = bigz_compare_to(&q, "-59733647658934344963858", 10);
  EXPECT_EQ(cmpq, 0);

  int cmpr = bigz_compare_to(&r, "0", 10);

  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, DivNegativeNegativeZeroRemainder) {
  struct bigz b1;
  bigz_create_from_string(&b1, "-278230124321795170179054969089584492590180", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "-4657845874580210210", 10);

  struct bigz q;
  bigz_create_empty(&q);

  struct bigz r;
  bigz_create_empty(&r);

  bigz_div(&q, &r, &b1, &b2);

  int cmpq = bigz_compare_to(&q, "59733647658934344963858", 10);
  EXPECT_EQ(cmpq, 0);

  int cmpr = bigz_compare_to(&r, "0", 10);
  EXPECT_EQ(cmpr, 0);

  bigz_destroy(&r);
  bigz_destroy(&q);
  bigz_destroy(&b2);
  bigz_destroy(&b1);
}

TEST(BigZ, GcdCoprime) {
  struct bigz b1;
  bigz_create_from_string(&b1, "1533918542734615245164352154619195752815244561235899341531", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "1533918542734615245164352154619195752815244561235899341532", 10);

  struct bigz b3;
  bigz_create_empty(&b3);

  bigz_gcd(&b3, &b1, &b2);

  int cmp = bigz_compare_to(&b3, "1", 10);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b3);
  bigz_destroy(&b2);
  bigz_destroy(&b1);

}

TEST(BigZ, GcdRsa768) {
  struct bigz b1;
  bigz_create_from_string(&b1, "d982ec7b440e2869d2535e51f91bacc3eb6eba042e106e6f875c3d17e53db65fffd6e4e9a36084ce60f83d754dd7f701", 16);

  struct bigz b2;
  bigz_create_from_string(&b2, "eebe6dd23ce7e99c0e2249fecc4418c34af74e418bfa714c3791828414ab18f32fd7e093062a49b030225cc845f99ab5", 16);

  struct bigz b3;
  bigz_create_empty(&b3);

  bigz_gcd(&b3, &b1, &b2);

  int cmp = bigz_compare_to(&b3, "1", 10);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b3);
  bigz_destroy(&b2);
  bigz_destroy(&b1);

}

TEST(BigZ, GcdNotCoprime) {
  struct bigz b1;
  bigz_create_from_string(&b1, "829029200588685010168228367991253899050528829049", 10);

  struct bigz b2;
  bigz_create_from_string(&b2, "92196793900174799975899442851691027900097961", 10);

  struct bigz b3;
  bigz_create_empty(&b3);

  bigz_gcd(&b3, &b1, &b2);

  int cmp = bigz_compare_to(&b3, "97713221", 10);
  EXPECT_EQ(cmp, 0);

  bigz_destroy(&b3);
  bigz_destroy(&b2);
  bigz_destroy(&b1);

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
