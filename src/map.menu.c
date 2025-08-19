#pragma bank 2

#include <stdio.h>

#include "map.h"
#include "player.h"
#include "sound.h"

#define MAP_MENU_X 0x00
#define MAP_MENU_Y 0x0E

#define CURSOR_TILE_ID 0x8Au
#define CURSOR_SPRITE_ID 16

#define CURSOR_X_SAVE 16 - 2
#define CURSOR_X_QUIT 16 + 7 * 8 - 2
#define CURSOR_Y 0x11 * 8

#define NAME_X 1
#define NAME_Y 0xF
#define CLASS_X 0xB
#define LEVEL_X 0x11

#define draw_number_at(n, x, y) do { \
  sprintf(buf, "%u", (n)); \
  core.draw_text(VRAM_WINDOW_XY((x), (y)), buf, 3); \
  } while(0)

MapMenu map_menu;

const palette_color_t main_menu_palette[] = {
  RGB_WHITE,
  RGB8(101, 128, 186),
  RGB8(3, 37, 135),
  RGB8(22, 6, 4),
};

static const Tilemap map_menu_tilemap = { 20, 18, BANK_1, tilemap_map_menu };

static void update_cursor(void) {
  uint8_t sx;
  const uint8_t sy = CURSOR_Y;

  if (map_menu.cursor == MAP_MENU_CURSOR_SAVE)
    sx = CURSOR_X_SAVE;
  else
    sx = CURSOR_X_QUIT;

  move_sprite(CURSOR_SPRITE_ID + 0, sx, sy);
  move_sprite(CURSOR_SPRITE_ID + 1, sx + 8, sy);
  move_sprite(CURSOR_SPRITE_ID + 2, sx, sy + 8);
  move_sprite(CURSOR_SPRITE_ID + 3, sx + 8, sy + 8);
}

static void hide_cursor(void) {
  move_sprite(CURSOR_SPRITE_ID + 0, 0, 0);
  move_sprite(CURSOR_SPRITE_ID + 1, 0, 0);
  move_sprite(CURSOR_SPRITE_ID + 2, 0, 0);
  move_sprite(CURSOR_SPRITE_ID + 3, 0, 0);
}

void update_map_menu_hp_sp(void) {
  core.print_fraction(VRAM_WINDOW_XY(0x6, 0x14), player.hp, player.max_hp);
  core.print_fraction(VRAM_WINDOW_XY(0x6, 0x15), player.sp, player.max_sp);
}

void update_map_menu_stats(void) {
  char buf[16];

  // Name
  sprintf(buf, player.name);
  core.draw_text(VRAM_WINDOW_XY(NAME_X, NAME_Y), buf, 8);

  // Class
  switch (player.player_class) {
  case CLASS_DRUID:
    core.draw_text(VRAM_WINDOW_XY(CLASS_X, NAME_Y), str_misc_druid_short, 4);
    break;
  case CLASS_FIGHTER:
    core.draw_text(VRAM_WINDOW_XY(CLASS_X, NAME_Y), str_misc_fighter_short, 4);
    break;
  case CLASS_MONK:
    core.draw_text(VRAM_WINDOW_XY(CLASS_X, NAME_Y), str_misc_monk_short, 4);
    break;
  case CLASS_SORCERER:
    core.draw_text(VRAM_WINDOW_XY(CLASS_X, NAME_Y), str_misc_sorcerer_short, 4);
    break;
  default:
    core.draw_text(VRAM_WINDOW_XY(CLASS_X, NAME_Y), "TST", 4);
  }

  // Level
  sprintf(buf, "%u", player.level);
  core.draw_text(VRAM_WINDOW_XY(LEVEL_X, NAME_Y), buf, 2);

  // EXP / Next Level
  sprintf(buf, "%u/%u", player.exp, player.next_level_exp);
  core.draw_text(VRAM_WINDOW_XY(0x4, 0x12), buf, 12);

  // SP/MP Label
  sprintf(buf, is_magic_class() ? "MP" : "SP");
  core.draw_text(VRAM_WINDOW_XY(0x2, 0x15), buf, 2);

  // HP & MP
  update_map_menu_hp_sp();

  // ATK, DEF, MATK, MDEF, AGL
  draw_number_at(player.atk_base, 0x6, 0x17);
  draw_number_at(player.def_base, 0x10, 0x17);
  draw_number_at(player.matk_base, 0x6, 0x18);
  draw_number_at(player.mdef_base, 0x10, 0x18);
  draw_number_at(player.agl_base, 0x10, 0x19);
}

void init_map_menu(void) {
  core.draw_tilemap(map_menu_tilemap, VRAM_WINDOW_XY(MAP_MENU_X, MAP_MENU_Y));
  map_menu.cursor = MAP_MENU_CURSOR_SAVE;

  set_sprite_tile(CURSOR_SPRITE_ID + 0, CURSOR_TILE_ID + 0u);
  set_sprite_tile(CURSOR_SPRITE_ID + 1, CURSOR_TILE_ID + 1u);
  set_sprite_tile(CURSOR_SPRITE_ID + 2, CURSOR_TILE_ID + 2u);
  set_sprite_tile(CURSOR_SPRITE_ID + 3, CURSOR_TILE_ID + 3u);

  set_sprite_prop(CURSOR_SPRITE_ID + 0, 0b00001100);
  set_sprite_prop(CURSOR_SPRITE_ID + 1, 0b00001100);
  set_sprite_prop(CURSOR_SPRITE_ID + 2, 0b00001100);
  set_sprite_prop(CURSOR_SPRITE_ID + 3, 0b00001100);

  hide_cursor();

  update_map_menu_stats();
}

void show_map_menu(void) {
  map_state = MAP_STATE_MENU;
  map_menu.state = MAP_MENU_OPEN;

  LCDC_REG |= 0b00001000;
  move_bkg(0, 14 * 8);

  core.load_bg_palette(main_menu_palette, 7, 1);
  clear_map_sprites();
  update_cursor();

  play_sound(sfx_next_round);
}

void hide_map_menu(void) {
  map_menu.state = MAP_MENU_CLOSED;
  core.load_bg_palette(textbox_palette, 7, 1);
  LCDC_REG &= 0b11110111;
  move_bkg(map_scroll_x, map_scroll_y);
  hide_cursor();
  play_sound(sfx_next_round);
}

void update_map_menu(void) {
  if (map_menu.state == MAP_MENU_CLOSED)
    return;

  if (was_pressed(J_START) || was_pressed(J_B) || was_pressed(J_A)) {
    play_sound(sfx_menu_move);
    hide_map_menu();
    return;
  }
}
