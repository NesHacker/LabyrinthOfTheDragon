#include <stdint.h>

#include "battle.h"
#include "core.h"
#include "map.h"
#include "stats.h"

uint8_t *_test = (void *)0xA000;
#define TEST(a, b) *_test++ = (a) - (b)

void reset_test(void) {
  for (uint8_t k = 0; k < 0xFF; k++) {
    *_test++ = 0xFF;
  }
  _test = (void *)0xA000;
}

void test_battle_init(void) {
  encounter.is_test = true;
  toggle_sprites();
  init_battle();
}

void test_stats(void) {
  game_state = GAME_STATE_TEST;
  reset_test();

  uint8_t base = 30;

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

void test_setup_encounter(MonsterLayout layout, TestDummyType type) {
  reset_encounter(layout);

  MonsterInstance *monster = encounter.monsters;

  switch (layout) {
  case MONSTER_LAYOUT_1:
    dummy_generator(monster, player.level, type);
    monster->id = 'A';
    break;
  case MONSTER_LAYOUT_2:
    dummy_generator(monster, player.level, type);
    monster->id = 'A';
    dummy_generator(++monster, player.level, type);
    monster->id = 'B';
    break;
  case MONSTER_LAYOUT_1M_2S:
  case MONSTER_LAYOUT_3S:
    dummy_generator(monster, player.level, type);
    monster->id = 'A';
    dummy_generator(++monster, player.level, type);
    monster->id = 'B';
    dummy_generator(++monster, player.level, type);
    monster->id = 'C';
    break;
  }
}

void test_combat_general(MonsterLayout layout, TestDummyType type) {
  init_test_player(20);
  test_setup_encounter(layout, type);
  test_battle_init();
}

void test_flee(void) {
  init_test_player(20);
  init_world_map();
  game_state = GAME_STATE_WORLD_MAP;

  // update_world_map();
  // draw_world_map();

  // test_combat_general(MONSTER_LAYOUT_1);
  // start_battle();

  // toggle_sprites();
  // init_battle();
}
