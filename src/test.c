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
  init_test_player(CLASS_SORCERER, 40);
  // grant_ability(ABILITY_0);
  grant_ability(ABILITY_0 | ABILITY_1 | ABILITY_2 | ABILITY_3);
  fill_inventory(5);

  SET_HAS_TORCH;
  // SET_MAGIC_KEYS(2);
  PASS_DOORS;
  DISABLE_ENCOUNTERS;

  // Monster *monster = encounter.monsters;
  // reset_encounter(MONSTER_LAYOUT_3S);
  // goblin_generator(monster, 10, C_TIER);
  // monster->id = 'A';
  // monster++;
  // goblin_generator(monster, 10, B_TIER);
  // monster->id = 'B';
  // monster++;
  // goblin_generator(monster, 10, A_TIER);
  // monster->id = 'C';
  // test_battle_init();


  // set_active_floor(&bank_floor1);
  // set_active_floor(&bank_floor2);
  // set_active_floor(&bank_floor3);
  // set_active_floor(&bank_floor4);

  set_active_floor(&bank_floor5);
  init_world_map();

  // TEST_SEED(62);
}

void test_battle(void) {
  reset_encounter(MONSTER_LAYOUT_2);
  Monster *monster = encounter.monsters;
  dragon_generator(monster++, 10, C_TIER);
  dragon_generator(monster++, 10, B_TIER);

  test_battle_init();
}
