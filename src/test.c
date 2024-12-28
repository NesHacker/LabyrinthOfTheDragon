#include <stdint.h>
#include <stdio.h>

#include "battle.h"
#include "core.h"
#include "floor.h"
#include "item.h"
#include "map.h"
#include "sound.h"
#include "stats.h"

uint8_t *_test = (void *)0xA000;
#define TEST(a, b) *_test++ = (a) - (b)

void reset_test(void) {
  for (uint8_t k = 0; k < 0xFF; k++) {
    *_test++ = 0xFF;
  }
  _test = (void *)0xA000;
}

void init_test_player(PlayerClass c, uint8_t level) {
  init_player(c);
  // init_player(CLASS_SORCERER);
  // grant_ability(ABILITY_ALL);
  set_player_level(level);
  sprintf(player.name, "Tester");
  player.message_speed = AUTO_PAGE_FAST;
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

  Monster *monster = encounter.monsters;

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

void fill_inventory(uint8_t amt) {
  Item *item = inventory;
  for (uint8_t k = 0; k < INVENTORY_LEN; k++, item++)
    item->quantity = amt;
}

void test_combat_general(MonsterLayout layout, TestDummyType type) {
  init_test_player(CLASS_TEST, 20);
  fill_inventory(3);
  test_setup_encounter(layout, type);
  // player.exp = get_exp(player.level + 1) - 1;
  // encounter.monsters->exp_tier = S_TIER;
  test_battle_init();
}

void test_flee(void) {
  init_test_player(CLASS_TEST, 20);
  init_world_map();
  game_state = GAME_STATE_WORLD_MAP;
}

void test_big_map(void) {
  init_test_player(CLASS_TEST, 20);

  set_active_floor(&floor_test);
  // set_active_floor(&floor_test2);
  init_world_map();

  game_state = GAME_STATE_WORLD_MAP;
}

