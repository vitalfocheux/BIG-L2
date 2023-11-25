#ifndef BIG_H
#define BIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bign {
  uint32_t *data;
  size_t capacity;
  size_t size;
};


void bign_create_empty(struct bign *self);
void bign_create_from_value(struct bign *self, uint32_t val);
void bign_create_from_string(struct bign *self, const char *str);

void bign_copy_from_other(struct bign *self, const struct bign *other);
void bign_move_from_other(struct bign *self, struct bign *other);

void bign_destroy(struct bign *self);

void bign_print(const struct bign *self);

int bign_cmp(const struct bign *lhs, const struct bign *rhs);
int bign_cmp_zero(const struct bign *self);

void bign_add(struct bign *self, const struct bign *lhs, const struct bign *rhs);
void bign_sub(struct bign *self, const struct bign *lhs, const struct bign *rhs);

void bign_mul(struct bign *self, const struct bign *lhs, const struct bign *rhs);
void bign_mul_karatsuba(struct bign *self, const struct bign *lhs, const struct bign *rhs);
void bign_mul_short(struct bign *self, const struct bign *lhs, uint32_t rhs);

void bign_div_short(struct bign *quo, uint32_t *rem, const struct bign *lhs, uint32_t rhs);
void bign_div(struct bign *quo, struct bign *rem, const struct bign *lhs, const struct bign *rhs);

void bign_exp(struct bign *self, const struct bign *lhs, const struct bign *rhs);


struct bigz {
  bool positive;
  struct bign n;
};


void bigz_create_empty(struct bigz *self);
void bigz_create_from_value(struct bigz *self, int32_t val);
void bigz_create_from_string(struct bigz *self, const char *str, unsigned base);

void bigz_copy_from_other(struct bigz *self, const struct bigz *other);
void bigz_move_from_other(struct bigz *self, struct bigz *other);

void bigz_destroy(struct bigz *self);

void bigz_print(const struct bigz *self);

int bigz_cmp(const struct bigz *lhs, const struct bigz *rhs);
int bigz_cmp_zero(const struct bigz *self);

void bigz_add(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs);
void bigz_sub(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs);

void bigz_mul(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs);
void bigz_div(struct bigz *quo, struct bigz *rem, const struct bigz *lhs, const struct bigz *rhs);

void bigz_gcd(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs);

#ifdef __cplusplus
}
#endif


#endif // BIG_H
