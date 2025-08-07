#include <rand.h>
#include <stdint.h>
#include <stdio.h>

#include "battle.h"
#include "core.h"
#include "encounter.h"
#include "floor.h"
#include "item.h"
#include "map.h"
#include "sound.h"
#include "stats.h"

#define SET_MAGIC_KEYS(n) do { \
    player.got_magic_key = true; \
    player.magic_keys = (n); \
  } while (0)

#define SET_HAS_TORCH player.has_torch = true

#define TEST_SEED(s) do {\
  init_random = false; \
  initarand(s); \
  } while(0)

#define DISABLE_ENCOUNTERS disable_encounters = true

#define PASS_DOORS pass_doors = true

void init_test_player(PlayerClass c, uint8_t level) {
  init_player(c);
  set_player_level(level);
  sprintf(player.name, "Tester");
  player.message_speed = AUTO_PAGE_MED;
}

void test_battle_init(void) {
  encounter.is_test = true;
  toggle_sprites();
  init_battle();
}

void fill_inventory(uint8_t amt) {
  Item *item = inventory;
  for (uint8_t k = 0; k < INVENTORY_LEN; k++, item++)
    item->quantity = amt;
}

void test_level(void) {
  init_test_player(CLASS_SORCERER, 90);
  // grant_ability(ABILITY_0);

  grant_ability(
  ABILITY_ALL
    // ABILITY_0
  // | ABILITY_1
  // | ABILITY_2
  // | ABILITY_3
  // | ABILITY_4
  // | ABILITY_5
  );
  // fill_inventory(5);

  SET_HAS_TORCH;
  SET_MAGIC_KEYS(9);
  PASS_DOORS;
  DISABLE_ENCOUNTERS;

  // set_active_floor(&bank_floor1);
  // set_active_floor(&bank_floor2);
  // set_active_floor(&bank_floor3);
  // set_active_floor(&bank_floor4);
  // set_active_floor(&bank_floor5);
  // set_active_floor(&bank_floor6);
  // set_active_floor(&bank_floor7);
  set_active_floor(&bank_floor8);
  init_world_map();
  game_state = GAME_STATE_WORLD_MAP;

  // TEST_SEED(62);
}

void test_battle(void) {
  init_test_player(CLASS_TEST, 90);
  reset_encounter(MONSTER_LAYOUT_1);
  fill_inventory(9);

  Monster *monster = encounter.monsters;
  gelatinous_cube_generator(monster, 10, C_TIER);
  monster->id = 'A';
  monster++;

  test_battle_init();
}
