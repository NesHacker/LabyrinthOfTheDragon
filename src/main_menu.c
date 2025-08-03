#pragma bank 1

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "core.h"
#include "main_menu.h"
#include "player.h"


/**
 * Top level state for the main menu.
 */
MainMenuState main_menu_state = TITLE;
uint8_t cursor = 0;


Timer menu_walk_timer;
uint8_t menu_walk_frame;

Tilemap title_screen_tilemap = { 20, 18, 1, tilemap_title_screen };
Tilemap save_select_tilemap = { 20, 18, 1, tilemap_save_select };

/**
 * Initializes the save select screen.
 */
void init_save_select(void) {
  DISPLAY_OFF;

  // TODO Need to load data from Save RAM and update graphics appropriately

  // Load the save select screen tilemap
  core.draw_tilemap(save_select_tilemap, VRAM_BACKGROUND);

  // Reset the cursor sprite and state
  cursor = SAVE_SELECT_CURSOR_SAVE1;
  set_bkg_palette(4, 3, save_1_selected_palettes);
  set_sprite_palette(0, 1, save_1_selected_palettes);
  set_sprite_tile(SPRITE_CURSOR, 0x9E);
  set_sprite_prop(SPRITE_CURSOR, 0x08);
  move_sprite(SPRITE_CURSOR, 22, 52);

  // Set up the animated "hero" sprites for each save file
  set_sprite_palette(1, 4, hero_palettes);

  // Hero 1
  set_sprite_tile(SPRITE_HERO1, 0x00);
  set_sprite_tile(SPRITE_HERO1 + 1, 0x01);
  set_sprite_tile(SPRITE_HERO1 + 2, 0x10);
  set_sprite_tile(SPRITE_HERO1 + 3, 0x11);

  set_sprite_prop(SPRITE_HERO1, 0x02);
  set_sprite_prop(SPRITE_HERO1 + 1, 0x02);
  set_sprite_prop(SPRITE_HERO1 + 2, 0x02);
  set_sprite_prop(SPRITE_HERO1 + 3, 0x02);

  move_sprite(SPRITE_HERO1, HERO1_X, HERO1_Y);
  move_sprite(SPRITE_HERO1 + 1, HERO1_X + 8, HERO1_Y);
  move_sprite(SPRITE_HERO1 + 2, HERO1_X, HERO1_Y + 8);
  move_sprite(SPRITE_HERO1 + 3, HERO1_X + 8, HERO1_Y + 8);

  // Hero 2
  set_sprite_tile(SPRITE_HERO2, 0x40);
  set_sprite_tile(SPRITE_HERO2 + 1, 0x41);
  set_sprite_tile(SPRITE_HERO2 + 2, 0x50);
  set_sprite_tile(SPRITE_HERO2 + 3, 0x51);

  set_sprite_prop(SPRITE_HERO2, 0x01);
  set_sprite_prop(SPRITE_HERO2 + 1, 0x01);
  set_sprite_prop(SPRITE_HERO2 + 2, 0x01);
  set_sprite_prop(SPRITE_HERO2 + 3, 0x01);

  move_sprite(SPRITE_HERO2, HERO2_X, HERO2_Y);
  move_sprite(SPRITE_HERO2 + 1, HERO2_X + 8, HERO2_Y);
  move_sprite(SPRITE_HERO2 + 2, HERO2_X, HERO2_Y + 8);
  move_sprite(SPRITE_HERO2 + 3, HERO2_X + 8, HERO2_Y + 8);

  // Hero 3
  set_sprite_tile(SPRITE_HERO3, 0x60);
  set_sprite_tile(SPRITE_HERO3 + 1, 0x61);
  set_sprite_tile(SPRITE_HERO3 + 2, 0x70);
  set_sprite_tile(SPRITE_HERO3 + 3, 0x71);

  set_sprite_prop(SPRITE_HERO3, 0x01);
  set_sprite_prop(SPRITE_HERO3 + 1, 0x01);
  set_sprite_prop(SPRITE_HERO3 + 2, 0x01);
  set_sprite_prop(SPRITE_HERO3 + 3, 0x01);

  move_sprite(SPRITE_HERO3, HERO3_X, HERO3_Y);
  move_sprite(SPRITE_HERO3 + 1, HERO3_X + 8, HERO3_Y);
  move_sprite(SPRITE_HERO3 + 2, HERO3_X, HERO3_Y + 8);
  move_sprite(SPRITE_HERO3 + 3, HERO3_X + 8, HERO3_Y + 8);

  init_timer(menu_walk_timer, 12);

  DISPLAY_ON;
}

void move_save_select_cursor(void) {
  if (was_pressed(J_UP)) {
    cursor = (cursor == SAVE_SELECT_CURSOR_SAVE1)
      ? SAVE_SELECT_CURSOR_ERASE
      : cursor - 1;
  } else if (was_pressed(J_DOWN)) {
    cursor = (cursor == SAVE_SELECT_CURSOR_ERASE)
      ? SAVE_SELECT_CURSOR_SAVE1
      : cursor + 1;
  } else {
    return;
  }

  switch (cursor) {
  case SAVE_SELECT_CURSOR_SAVE1:
    move_sprite(SPRITE_CURSOR, 22, 52);
    set_bkg_palette(4, 3, save_1_selected_palettes);
    for (uint8_t s = 0; s < 4; s++) {
      set_sprite_prop(SPRITE_HERO1 + s, 0x02);
      set_sprite_prop(SPRITE_HERO2 + s, 0x01);
      set_sprite_prop(SPRITE_HERO3 + s, 0x01);
    }
    break;
  case SAVE_SELECT_CURSOR_SAVE2:
    move_sprite(SPRITE_CURSOR, 22, 84);
    set_bkg_palette(4, 3, save_2_selected_palettes);
    for (uint8_t s = 0; s < 4; s++) {
      set_sprite_prop(SPRITE_HERO1 + s, 0x01);
      set_sprite_prop(SPRITE_HERO2 + s, 0x03);
      set_sprite_prop(SPRITE_HERO3 + s, 0x01);
    }
    break;
  case SAVE_SELECT_CURSOR_SAVE3:
    move_sprite(SPRITE_CURSOR, 22, 116);
    set_bkg_palette(4, 3, save_3_selected_palettes);
    for (uint8_t s = 0; s < 4; s++) {
      set_sprite_prop(SPRITE_HERO1 + s, 0x01);
      set_sprite_prop(SPRITE_HERO2 + s, 0x01);
      set_sprite_prop(SPRITE_HERO3 + s, 0x04);
    }
    break;
  case SAVE_SELECT_CURSOR_ERASE:
    move_sprite(SPRITE_CURSOR, 100, 143);
    set_bkg_palette(4, 3, save_none_selected_palettes);
    for (uint8_t s = 0; s < 4; s++) {
      set_sprite_prop(SPRITE_HERO1 + s, 0x01);
      set_sprite_prop(SPRITE_HERO2 + s, 0x01);
      set_sprite_prop(SPRITE_HERO3 + s, 0x01);
    }
    break;
  }
}

void update_save_select_sprites(void) {
  if (update_timer(menu_walk_timer)) {
    reset_timer(menu_walk_timer);
    menu_walk_frame ^= 1;
  }

  const uint8_t walk_tile_offset = menu_walk_frame << 1;
  const uint8_t hero1_offset = (cursor == SAVE_SELECT_CURSOR_SAVE1)
    ? walk_tile_offset
    : 0;
  const uint8_t hero2_offset = (cursor == SAVE_SELECT_CURSOR_SAVE2)
    ? walk_tile_offset
    : 0;
  const uint8_t hero3_offset = (cursor == SAVE_SELECT_CURSOR_SAVE3)
    ? walk_tile_offset
    : 0;

  set_sprite_tile(SPRITE_HERO1 + 0, 0x00 + hero1_offset);
  set_sprite_tile(SPRITE_HERO1 + 1, 0x01 + hero1_offset);
  set_sprite_tile(SPRITE_HERO1 + 2, 0x10 + hero1_offset);
  set_sprite_tile(SPRITE_HERO1 + 3, 0x11 + hero1_offset);

  set_sprite_tile(SPRITE_HERO2 + 0, 0x40 + hero2_offset);
  set_sprite_tile(SPRITE_HERO2 + 1, 0x41 + hero2_offset);
  set_sprite_tile(SPRITE_HERO2 + 2, 0x50 + hero2_offset);
  set_sprite_tile(SPRITE_HERO2 + 3, 0x51 + hero2_offset);

  set_sprite_tile(SPRITE_HERO3 + 0, 0x60 + hero3_offset);
  set_sprite_tile(SPRITE_HERO3 + 1, 0x61 + hero3_offset);
  set_sprite_tile(SPRITE_HERO3 + 2, 0x70 + hero3_offset);
  set_sprite_tile(SPRITE_HERO3 + 3, 0x71 + hero3_offset);
}



const uint8_t fire_tiles[] = {
  // Frame 1
                    0x00, 0x01,
                    0x02, 0x03,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  // Frame 2
                    0x08, 0x09,
                    0x0A, 0x0B,
  0x07, 0x07, 0x07, 0x0C, 0x0D, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  // Frame 3
                    0x10, 0x11,
                    0x12, 0x13,
  0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
  0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,
  // Frame 4
                    0x24, 0x25,
                    0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  // Frame 5
                    0x38, 0x39,
                    0x3A, 0x3B,
  0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43,
  0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B,
  // Frame "Off"
                    0x07, 0x07,
                    0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
};

#define FIRE_FRAME_SPRITE_0 0
#define FIRE_FRAME_SPRITES 20

// 1 - 72, 112
// 5 - 48, 128

const uint8_t fire_sprite_x[FIRE_FRAME_SPRITES] = {
  72, 80,
  72, 80,
  48, 56, 64, 72, 80, 88, 96, 104,
  48, 56, 64, 72, 80, 88, 96, 104,
};

const uint8_t fire_sprite_y[FIRE_FRAME_SPRITES] = {
  112, 112,
  120, 120,
  128, 128, 128, 128, 128, 128, 128, 128,
  136, 136, 136, 136, 136, 136, 136, 136,
};

const uint8_t fire_frames[] = {
  0, 1,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  END
};

Timer fire_timer;
uint8_t fire_frame_idx = 0;

static void init_fire_animation(void) {
  init_timer(fire_timer, 6);
  fire_frame_idx = 0;

  for (uint8_t k = 0; k < FIRE_FRAME_SPRITES; k++) {
    const uint8_t id = FIRE_FRAME_SPRITE_0 + k;
    set_sprite_tile(id, fire_tiles[k]);
    set_sprite_prop(id, 0x08);
    move_sprite(id, fire_sprite_x[k] + 8, fire_sprite_y[k] + 16);
  }
}

static void update_fire_animation(void) {
  if (fire_frames[fire_frame_idx] == END || !update_timer(fire_timer))
    return;

  reset_timer(fire_timer);
  fire_frame_idx++;

  uint8_t fire_frame_offset = fire_frames[fire_frame_idx];
  if (fire_frame_offset == END) {
    fire_frame_offset = 5;
  }

  for (uint8_t k = 0; k < FIRE_FRAME_SPRITES; k++) {
    const uint8_t tile_offset = fire_frame_offset * FIRE_FRAME_SPRITES;
    set_sprite_tile(FIRE_FRAME_SPRITE_0 + k, fire_tiles[tile_offset + k]);
  }
}

static const palette_color_t fg_palettes[] = {
  // 1 - Fire
  RGB_BLACK,
  RGB8(252, 216, 0),
  RGB8(252, 108, 0),
  RGB_WHITE,
  // 2 - Smoke
  RGB_BLACK,
  RGB8(91, 91, 91),
  RGB8(156, 156, 156),
  RGB_WHITE,
};

static const palette_color_t palettes[] = {
   // Palette 1 - Title
  RGB8(251, 242, 54),
  RGB8(48, 96, 130),
  RGB8(172, 50, 50),
  RGB_BLACK,
  // Palette 2 - Face
  RGB8(252, 216, 0),
  RGB8(154, 32, 24),
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  // Palette 3 - Wings
  RGB8(154, 32, 24),
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  RGB_BLACK,
  // Palette 4 - Outer Body
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  RGB8(9, 4, 4),
  RGB_BLACK,
  // Palette 5 - Head
  RGB8(154, 32, 24),
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  RGB_BLACK,
  // Palette 6 - PRESS START
  // RGB8(251, 242, 54),
  RGB8(200, 0, 200),
  RGB_BLACK,
  RGB_BLACK,
  RGB_BLACK,
};




void init_main_menu(void) {
  DISPLAY_OFF;

  // Load tiles
  core.load_font();
  core.load_all_heros();
  core.load_title_tiles();

  core.load_bg_palette(palettes, 0, 6);
  core.load_sprite_palette(fg_palettes, 0, 2);

  // Load tilesets and palettes
  scroll_bkg(0, 0);
  SWITCH_ROM(1);
  core.draw_tilemap(title_screen_tilemap, VRAM_BACKGROUND);

  // Initialize menu state
  cursor = 0;

  init_fire_animation();

  DISPLAY_ON;
}

void update_main_menu(void) {
  switch (main_menu_state) {
  case TITLE:
    update_fire_animation();
    if (was_pressed(J_A | J_B | J_START)) {
      main_menu_state = SAVE_SELECT;
      init_save_select();
    }
    break;
  case SAVE_SELECT:
    move_save_select_cursor();
    update_save_select_sprites();

    break;
  }
}

void draw_main_menu(void) {
}
