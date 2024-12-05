#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "data.h"
#include "joypad.h"
#include "palette.h"
#include "util.h"

#define SPRITE_CURSOR 0



typedef enum BattleState {
  BATTLE_STATE_MAIN_MENU,
  BATTLE_STATE_FIGHT,
  BATTLE_STATE_SKILL,
  BATTLE_STATE_TEXT,
} BattleState;

typedef enum BattleCursor {
  BATTLE_CURSOR_FIGHT,
  BATTLE_CURSOR_MONSTER_MANUAL,
  BATTLE_CURSOR_SKILL,
  BATTLE_CURSOR_RUN,
} BattleCursor;


BattleState battle_state;
BattleCursor battle_cursor;

inline void move_cursor_sprite(uint8_t col, uint8_t row) {
  const uint8_t x = (col + 1) * 8;
  const uint8_t y = (row + 2) * 8 - 1;
  move_sprite(SPRITE_CURSOR, x, y);
}

void move_cursor(BattleCursor c) {
  switch (c) {
  case BATTLE_CURSOR_FIGHT:
    move_cursor_sprite(9, 14);
    break;
  case BATTLE_CURSOR_MONSTER_MANUAL:
    move_cursor_sprite(15, 14);
    break;
  case BATTLE_CURSOR_SKILL:
    move_cursor_sprite(9, 16);
    break;
  case BATTLE_CURSOR_RUN:
    move_cursor_sprite(15, 16);
    break;
  }
  battle_cursor = c;
}



palette_color_t default_palette[] = {
  RGB_WHITE,
  RGB8(200, 200, 200),
  RGB8(120, 120, 120),
  RGB_BLACK,
};

palette_color_t battle_text_box_palettes[] = {
  // Empty pane text box palette
  RGB_WHITE,
  RGB8(166, 146, 124),
  RGB8(175, 125, 78),
  RGB_BLACK,
  // Active pane text box palette
  RGB_WHITE,
  RGB8(227, 209, 189),
  RGB8(175, 125, 78),
  RGB_BLACK,
};

#define BORDER_CORNER 0x90
#define BORDER_HORIZ  0x91
#define BORDER_VERT   0x92

typedef enum BattleMenu {
  BATTLE_MENU_MAIN,
  BATTLE_MENU_TEXT,
  BATTLE_MENU_FIGHT,
} BattleMenu;


void fight_menu_isr(void) {
  if (battle_state == BATTLE_STATE_FIGHT)
    SCY_REG = 48;
}

void skill_menu_isr(void) {
  if (battle_state == BATTLE_STATE_SKILL)
    SCY_REG = 48;
}

/**
 * Draws one of the three major battle map menus to the given address in VRAM.
 * The battle maps are drawn using a single tilemap that contains all three.
 * @param m The battle menu to draw.
 * @param vram Pointer in VRAM to write the menu tiles.
 */
void draw_battle_menu(BattleMenu m, uint8_t *vram) {
  const uint8_t w = 20;
  uint8_t h = 6;
  const uint8_t *src = tilemap_battle_menus;

  switch (m) {
  case BATTLE_MENU_MAIN:
    break;
  case BATTLE_MENU_TEXT:
    src = tilemap_battle_menus + w * 6 * 2;
    break;
  case BATTLE_MENU_FIGHT:
    src = tilemap_battle_menus + w * 12 * 2;
    h = 10;
    break;
  }

  for (uint8_t y = 0; y < h; y++, vram += 0x20 - w) {
    for (uint8_t x = 0; x < w; x++, vram++) {
      // Skip "transparent" tiles (tile 0x81 is reserved for this)
      if (*src == 0x81) {
        src += 2;
        continue;
      }
      VBK_REG = VBK_TILES;
      *vram = *src++;
      VBK_REG = VBK_ATTRIBUTES;
      *vram = *src++;
    }
  }
}

void init_battle(void) {
  lcd_off();

  // Load battle assets (tiles, palettes, etc.)
  update_bg_palettes(0, 1, default_palette);
  update_bg_palettes(6, 2, battle_text_box_palettes);
  update_sprite_palettes(7, 1, battle_text_box_palettes + 4);

  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_font, VRAM_SHARED_TILES);

  clear_background();
  move_win(0, 144);

  // TODO Draw the monster backgrounds & stat blocks

  // Draw the battle menus.
  draw_battle_menu(BATTLE_MENU_MAIN, VRAM_BACKGROUND_XY(0, 12));
  draw_battle_menu(BATTLE_MENU_FIGHT, VRAM_BACKGROUND_XY(0, 18));
  draw_battle_menu(BATTLE_MENU_TEXT, VRAM_WINDOW_XY(0, 0));

  // Setup cursor sprite
  set_sprite_tile(0, 0xFE);
  set_sprite_prop(SPRITE_CURSOR, 0x0F);
  move_cursor(BATTLE_CURSOR_FIGHT);

  lcd_on();
}

void handle_main_menu(void) {
  switch (battle_cursor) {
  case BATTLE_CURSOR_FIGHT:
    if (was_pressed(J_RIGHT))
      move_cursor(BATTLE_CURSOR_MONSTER_MANUAL);
    else if (was_pressed(J_DOWN))
      move_cursor(BATTLE_CURSOR_SKILL);
    break;
  case BATTLE_CURSOR_SKILL:
    if (was_pressed(J_RIGHT))
      move_cursor(BATTLE_CURSOR_RUN);
    else if (was_pressed(J_UP))
      move_cursor(BATTLE_CURSOR_FIGHT);
    break;
  case BATTLE_CURSOR_MONSTER_MANUAL:
    if (was_pressed(J_LEFT))
      move_cursor(BATTLE_CURSOR_FIGHT);
    else if (was_pressed(J_DOWN))
      move_cursor(BATTLE_CURSOR_RUN);
    break;
  case BATTLE_CURSOR_RUN:
    if (was_pressed(J_LEFT))
      move_cursor(BATTLE_CURSOR_SKILL);
    else if (was_pressed(J_UP))
      move_cursor(BATTLE_CURSOR_MONSTER_MANUAL);
    break;
  }
}


void update_battle(void) {
  switch (battle_state) {
  case BATTLE_STATE_MAIN_MENU:
    handle_main_menu();
    break;
  }
  // Need to attach ISRs based on state...
  // CRITICAL {
  //   LYC_REG = 17;
  //   STAT_REG = STATF_LYC;
  //   add_LCD(text_menu_isr);
  //   add_LCD(nowait_int_handler);
  // }
  // set_interrupts(IE_REG | LCD_IFLAG);
}

void draw_battle(void) {
  move_bkg(0, 0);
}
