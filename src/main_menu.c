#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "global.h"
#include "joypad.h"
#include "main_menu.h"
#include "util.h"

#include "tiles.bank01.h"
#include "tilemaps.bank00.h"

/**
 * Top level state for the main menu.
 */
MainMenuState main_menu_state = TITLE;

/**
 * Initializes the save select screen.
 */
void init_save_select(void) {
  lcd_off();

  // TODO Need to load data from Save RAM and update graphics appropriately
  
  // Load the save select screen tilemap
  load_screen(0, tilemap_save_select);
  
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

  init_timer(walk_timer, 12);

  lcd_on();
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
  if (update_timer(walk_timer)) {
    reset_timer(walk_timer);
    walk_frame ^= 1;
  }

  const uint8_t walk_tile_offset = walk_frame << 1;
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

void init_main_menu(void) {
  lcd_off();
  
  // Load tiles
  VBK_REG = VBK_BANK_0;
  load_tile_page(1, tile_data_hero, VRAM_SPRITE_TILES);
  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_font, VRAM_SHARED_TILES);

  // Load tilesets and palettes
  scroll_bkg(0, 0);
  load_screen(0, tilemap_title_screen);
  set_bkg_palette(0, 8, main_menu_palettes);
  
  // Initialize menu state
  cursor = 0;

  lcd_on();
}

void update_main_menu(void) {
  switch (main_menu_state) {
  case TITLE:
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
