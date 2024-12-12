/*
 * Bank 3 contains the majority of battle related tables and routines.
 * Level based tables are defined in `src/tables.c` and use roughly 6KB of data.
 */

#pragma bank 3

#include <stdint.h>
#include <gbdk/incbin.h>

const uint8_t attack_roll_target[65] = {
  50, 54, 58, 63, 67, 72, 76, 81,
  85, 89, 94, 98, 103, 107, 112, 116,
  121, 125, 129, 134, 138, 143, 147, 152,
  156, 160, 165, 169, 174, 178, 183, 187,
  192, 193, 195, 196, 198, 199, 201, 203,
  204, 206, 207, 209, 211, 212, 214, 215,
  217, 219, 220, 222, 223, 225, 227, 228,
  230, 231, 233, 235, 236, 238, 239, 241,
  243,
};

const uint8_t damage_roll_modifier[16] = {
  6, 8, 10, 12, 13, 14, 15, 16,
  16, 17, 18, 19, 20, 22, 24, 26,
};

const uint8_t xp_mod[16] = {
  0, 2, 5, 9, 12, 14, 15, 16,
  16, 17, 18, 20, 23, 27, 30, 32,
};

INCBIN(tilemap_battle_menus, "res/tilemaps/battle_menus.tilemap")
INCBIN(tilemap_battle_monster_layouts, "res/tilemaps/battle_monster_layouts.tilemap")
