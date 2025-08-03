#pragma bank 1

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdio.h>

#include "core.h"
#include "floor.h"
#include "hero_select.h"
#include "player.h"
#include "map.h"

#define HERO_OFFSET_X 8 + 33
#define HERO_OFFSET_Y 78 + 8

const palette_color_t selection_bg_palettes[] = {
  RGB_WHITE,
  RGB8(81, 108, 186),
  RGB8(3, 37, 135),
  RGB8(22, 6, 4),
};

const palette_color_t selection_palettes[] = {
  // 5 - Unselected Hero
  RGB8(40, 40, 40),
  RGB8(80, 80, 80),
  RGB8(140, 140, 140),
  RGB8(20, 20, 20),
};

static const uint8_t hero_tiles[] = {
  0x00, 0x01, 0x10, 0x11, // Druid
  0x20, 0x21, 0x30, 0x31, // Fighter
  0x40, 0x41, 0x50, 0x51, // Monk
  0x60, 0x61, 0x70, 0x71, // Sorcerer
};

static const uint8_t hero_sprite_x[] = {
  0, 8, 0, 8,
  24, 32, 24, 32,
  48, 56, 48, 56,
  72, 80, 72, 80,
};

static const uint8_t hero_sprite_y[] = {
  0, 0, 8, 8,
  0, 0, 8, 8,
  0, 0, 8, 8,
  0, 0, 8, 8,
};

static const uint8_t hero_attr[] = {
  0, 0, 0, 0,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
};

Tilemap hero_select_tilemap = { 20, 18, 1, tilemap_hero_select };

uint8_t selected_hero = 0;
Timer selected_walk_timer;
uint8_t selected_walk_frame = 0;

void init_hero_select(void) NONBANKED {
  DISPLAY_OFF;

  core.load_font();
  core.load_all_heros();

  core.load_sprite_palette(hero_colors, 0, 4);
  core.load_sprite_palette(selection_palettes, 4, 2);
  core.load_bg_palette(selection_bg_palettes, 0, 1);

  core.draw_tilemap(hero_select_tilemap, VRAM_BACKGROUND);


  for (uint8_t k = 0; k < 16; k++) {
    set_sprite_tile(k, hero_tiles[k]);
    set_sprite_prop(k, hero_attr[k]);
    move_sprite(k,
      hero_sprite_x[k] + HERO_OFFSET_X,
      hero_sprite_y[k] + HERO_OFFSET_Y);
  }

  selected_hero = 0;
  selected_walk_frame = 0;
  init_timer(selected_walk_timer, 12);

  DISPLAY_ON;
}

void start_game(void) NONBANKED {
  DISPLAY_OFF;

  for (uint8_t k = 0; k < 16; k++)
    move_sprite(k, 0, 0);

  init_player(selected_hero);
  set_active_floor(&bank_floor1);
  init_world_map();

  game_state = GAME_STATE_WORLD_MAP;
}

void change_hero(void) {
  for (uint8_t k = 0; k < 16; k++) {
    set_sprite_prop(k, 4);
    set_sprite_tile(k, hero_tiles[k]);
  }

  for (uint8_t j = 0; j < 4; j++) {
    set_sprite_prop(selected_hero * 4 + j, selected_hero);
  }

  selected_walk_frame = 0;
}

void update_hero_select(void) NONBANKED {
  if (was_pressed(J_LEFT)) {
    selected_hero = selected_hero == 0 ? 3 : selected_hero - 1;
    change_hero();
  } else if (was_pressed(J_RIGHT)) {
    selected_hero = (selected_hero + 1) & 0x03;
    change_hero();
  } else if (was_pressed(J_START)) {
    start_game();
    return;
  }

  if (!update_timer(selected_walk_timer))
    return;

  reset_timer(selected_walk_timer);
  selected_walk_frame = (selected_walk_frame + 1) & 0x1;

  for (uint8_t k = 0; k < 4; k++) {
    uint8_t id = k + selected_hero * 4;
    uint8_t offset = selected_walk_frame ? 2 : 0;
    set_sprite_tile(id, hero_tiles[id] + offset);
  }
}
