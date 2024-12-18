/**
 * [Core] Bank 3 - Battle System
 */

#pragma bank 3

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gbdk/incbin.h>
#include <stdint.h>

#include "../src/core.h"

const palette_color_t monster_death_colors[] = {
  // Palette 0
  RGB8(255, 255, 255),
  RGB8(207, 66, 66),
  RGB8(128, 41, 92),
  RGB8(42, 9, 69),
  // Palette 1
  RGB8(255, 255, 255),
  RGB8(207, 134, 134),
  RGB8(128, 41, 92),
  RGB8(42, 9, 69),
  // Palette 2
  RGB8(255, 255, 255),
  RGB8(207, 179, 179),
  RGB8(154, 95, 130),
  RGB8(42, 9, 69),
  // Palette 3
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(190, 144, 171),
  RGB8(82, 38, 119),
  // Palette 4
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(156, 123, 183),
  // Palette 5
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
};

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

const Tilemap monster_layout_1 = {
  20, 11, 5, tilemap_battle_monster_layouts
};

const Tilemap monster_layout_2 = {
  20, 11, 5, tilemap_battle_monster_layouts + 20 * 11 * 2 * 1
};

const Tilemap monster_layout_3s = {
  20, 11, 5, tilemap_battle_monster_layouts + 20 * 11 * 2 * 2
};

const Tilemap monster_layout_1m2s = {
  20, 11, 5, tilemap_battle_monster_layouts + 20 * 11 * 2 * 3
};

const Tilemap battle_menu_tilemap = {
  20, 7, 5, tilemap_battle_menus
};

const Tilemap battle_submenu_tilemap = {
  20, 7, 5, tilemap_battle_menus + 20 * 6 * 2,
};

const Tilemap battle_textbox_tilemap = {
  20, 7, 5, tilemap_battle_menus + 20 * 2 * 6 * 2,
};

