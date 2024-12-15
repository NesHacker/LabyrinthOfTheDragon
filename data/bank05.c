/**
 * [Data] Bank 5 - Stats & Battle Routines
 */
#pragma bank 5

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gbdk/incbin.h>
#include <stdint.h>

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

// Shoving some extra battle system data here to make BANK 3 more cozy

const palette_color_t data_battle_bg_colors[] = {
  // Palette 0 - Background / Textbox
  RGB_WHITE,
  RGB8(111, 127, 243),
  RGB8(60, 80, 60),
  RGB8(28, 28, 0),
  // Palette 1 - Monster 1
  RGB_WHITE,
  RGB8(209, 206, 107),
  RGB8(126, 73, 73),
  RGB8(0, 40, 51),
  // Palette 2 - Monster 2
  RGB_WHITE,
  RGB8(209, 206, 107),
  RGB8(126, 73, 73),
  RGB8(0, 40, 51),
  // Palette 3 - Monster 3
  RGB_WHITE,
  RGB8(209, 206, 107),
  RGB8(126, 73, 73),
  RGB8(0, 40, 51),
  // Palette 4 - HP Normal
  RGB_WHITE,
  RGB8(150, 200, 150),
  RGB8(80, 120, 80),
  RGB8(0, 32, 0),
  // Palette 5 - HP Critical
  RGB_WHITE,
  RGB8(200, 150, 150),
  RGB8(120, 80, 80),
  RGB8(32, 0, 0),
  // Palette 6 - Buff
  RGB8(40, 150, 40),
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB_BLACK,
  // Palette 7 - Debuff
  RGB8(150, 40, 40),
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB_BLACK,
};

const palette_color_t data_battle_sprite_colors[] = {
  // Palette 0 - Cursor
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 1 - UNUSED
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 2 - UNUSED
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 3 - UNUSED
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 4 - UNUSED
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 5 - UNUSED
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 6 - UNUSED
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // Palette 7 - CURSOR
  RGB_WHITE,
  RGB_BLACK,
  RGB_DARKGRAY,
  RGB_WHITE,
};

INCBIN(tilemap_battle_menus, "res/tilemaps/battle_menus.tilemap")
INCBIN(tilemap_battle_monster_layouts, "res/tilemaps/battle_monster_layouts.tilemap")

