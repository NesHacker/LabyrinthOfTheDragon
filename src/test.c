#include <stdint.h>
#include "core.h"
#include "stats.h"

uint8_t *_test = (void *)0xA000;
#define TEST(a, b) *_test++ = (a) - (b)

void reset_test(void) {
  for (uint8_t k = 0; k < 0xFF; k++) {
    *_test++ = 0xFF;
  }
  _test = (void *)0xA000;
}

void test_stats(void) {
  reset_test();

  uint8_t base = 30;

  TEST(blind_atk(base, C_TIER), base - 8);
  TEST(blind_atk(base, B_TIER), base - 12);
  TEST(blind_atk(base, A_TIER), base - 16);
  TEST(blind_atk(base, S_TIER), base - 22);
  TEST(blind_atk(20, S_TIER), 0);

  TEST(agl_down(base, C_TIER), base - 2);
  TEST(agl_down(base, B_TIER), base - 4);
  TEST(agl_down(base, A_TIER), base - 8);
  TEST(agl_down(base, S_TIER), base - 12);
  TEST(agl_down(11, S_TIER), 0);

  TEST(atk_down(base, C_TIER), 29);
  TEST(atk_down(base, B_TIER), 27);
  TEST(atk_down(base, A_TIER), 23);
  TEST(atk_down(base, S_TIER), 19);

  TEST(def_down(base, C_TIER), 29);
  TEST(def_down(base, B_TIER), 27);
  TEST(def_down(base, A_TIER), 23);
  TEST(def_down(base, S_TIER), 19);

  TEST(atk_up(base, C_TIER), 31);
  TEST(atk_up(base, B_TIER), 33);
  TEST(atk_up(base, A_TIER), 37);
  TEST(atk_up(base, S_TIER), 41);

  TEST(def_up(base, C_TIER), 31);
  TEST(def_up(base, B_TIER), 33);
  TEST(def_up(base, A_TIER), 37);
  TEST(def_up(base, S_TIER), 41);
}

