#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "battle.h"
#include "data.h"
#include "joypad.h"
#include "monster.h"
#include "palette.h"
#include "util.h"

BattleState battle_state;
BattleCursor battle_cursor;

// TODO Load this from the player model
PlayerSkill battle_player_skills[4] = {
  { 1, "SECONDWIND", "SKILL", 25, 30 },
  { 2, "ACTIONSURGE", "SKILL", 1, 2 },
  { 0, "-" },
  { 0, "-" },
};

// TODO Load this from the current party monster
MonsterMove battle_monster_moves[4] = {
  { 1, "TACKLE", "NORMAL", 19, 30 },
  { 2, "LEER", "NORMAL", 9, 10 },
  { 3, "BEERGUZZLE", "SPECIAL", 2, 3 },
  { 0, "-" }
};

/**
 * Default palettes for the battle system.
 */
palette_color_t battle_bg_palettes[] = {
  // 0 - Text / HP Normal
  RGB_WHITE,
  RGB8(100, 200, 100),
  RGB8(50, 120, 50),
  RGB_BLACK,
  // 1 - Text / HP Critical
  RGB_WHITE,
  RGB8(200, 100,  100),
  RGB8(120, 50, 50),
  RGB_BLACK,
  // 2 - Skull Border
  RGB_WHITE,
  RGB8(168, 172, 119),
  RGB8(120, 103, 193),
  RGB8(25, 58, 58),
  // 3 - Party Monster Palette
  RGB_WHITE,
  RGB8(200, 200, 200),
  RGB8(120, 120, 120),
  RGB_BLACK,
  // 4 - Enemy Monster Palette
  RGB_WHITE,
  RGB8(200, 200, 200),
  RGB8(120, 120, 120),
  RGB_BLACK,
  // 5 - Status Effect Positive
  RGB_WHITE,
  RGB8(200, 200, 200),
  RGB8(120, 120, 120),
  RGB_BLACK,
  // 6 - Status Effect Negative
  RGB_WHITE,
  RGB8(200, 200, 200),
  RGB8(120, 120, 120),
  RGB_BLACK,
  // 7 - Text box panel
  RGB_WHITE,
  RGB8(227, 209, 189),
  RGB8(175, 125, 78),
  RGB_BLACK,
};

/**
 * Moves the battle cursor to the given cursor position.
 */
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
  case BATTLE_CURSOR_MOVE1:
    move_cursor_sprite(5, 13);
    break;
  case BATTLE_CURSOR_MOVE2:
    move_cursor_sprite(5, 14);
    break;
  case BATTLE_CURSOR_MOVE3:
    move_cursor_sprite(5, 15);
    break;
  case BATTLE_CURSOR_MOVE4:
    move_cursor_sprite(5, 16);
    break;
  }
  battle_cursor = c;
}

/**
 * LCY interrupt handler for the fight and skill menus. This handler changes the
 * scroll-y position at a specific scanline to display a different part of the
 * background that contains the graphics for these menus.
 */
void fight_menu_isr(void) {
  SCY_REG = 0;
  if (battle_state == BATTLE_STATE_SKILL) {
    SCY_REG = 80;
    return;
  }
  if (battle_state == BATTLE_STATE_FIGHT) {
    SCY_REG = 80;
    return;
  }
}

/**
 * Draws one of the three major battle menus to the given address in VRAM.
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

void draw_battle_screen(void) {
  uint8_t *vram = VRAM_BACKGROUND;
  const uint8_t *src = tilemap_battle_screen;
  for (uint8_t y = 0; y < 28; y++, vram += 12) {
    for (uint8_t x = 0; x < 20; x++, vram++) {
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
  update_bg_palettes(0, 8, battle_bg_palettes);
  update_sprite_palettes(7, 1, battle_bg_palettes + 4 * 6);

  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_font, VRAM_SHARED_TILES);
  load_tiles(1, tile_battle, (void *)(0x9500), 3 * 16);

  clear_background();
  move_win(0, 144);

  // TODO Draw the monster backgrounds & stat blocks

  // Draw the battle menus.
  // draw_battle_menu(BATTLE_MENU_MAIN, VRAM_BACKGROUND_XY(0, 12));
  // draw_battle_menu(BATTLE_MENU_FIGHT, VRAM_BACKGROUND_XY(0, 18));

  // Draw the screen layout
  draw_battle_screen();
  draw_battle_menu(BATTLE_MENU_TEXT, VRAM_WINDOW_XY(0, 0));

  // Setup cursor sprite
  set_sprite_tile(0, 0xFE);
  set_sprite_prop(SPRITE_CURSOR, 0x0F);
  move_cursor(BATTLE_CURSOR_FIGHT);

  // TODO Load player skills and monster moves
  // TODO Abstract monster move loading to reload on change monster

  // Setup the LCD STAT interrupt for graphics effects
  CRITICAL {
    LYC_REG = 63;
    STAT_REG = STATF_LYC;
    add_LCD(fight_menu_isr);
    add_LCD(nowait_int_handler);
  }
  set_interrupts(IE_REG | LCD_IFLAG);

  lcd_on();
}

void cleanup_battle(void) {
  // CRITICAL {
  //   remove_LCD(fight_menu_isr);
  //   remove_LCD(nowait_int_handler);
  //   STAT_REG = 0;
  // }
  // set_interrupts(IE_REG & ~LCD_IFLAG);
}

void draw_move_text(uint8_t move_num, char *text) {
  uint8_t c = 0;
  uint8_t *vram = VRAM_BACKGROUND_XY(6, 23 + move_num);
  VBK_REG = VBK_TILES;
  while (*text != 0 && c < 13) {
    set_vram_byte(vram++, (*text++) + 0x80);
    c++;
  }
  while (c < 13) {
    set_vram_byte(vram++, 0xA0);
    c++;
  }
}

void load_move_names(void) {
  MonsterMove *move = battle_monster_moves;
  for (uint8_t k = 0; k < 4; k++, move++) {
    draw_move_text(k, move->name);
  }
}

void load_skill_names(void) {
  PlayerSkill *skill = battle_player_skills;
  for (uint8_t k = 0; k < 4; k++, skill++) {
    draw_move_text(k, skill->name);
  }
}

void handle_main_menu(void) {
  switch (battle_cursor) {
  case BATTLE_CURSOR_FIGHT:
    if (was_pressed(J_RIGHT))
      move_cursor(BATTLE_CURSOR_MONSTER_MANUAL);
    else if (was_pressed(J_DOWN))
      move_cursor(BATTLE_CURSOR_SKILL);
    else if (was_pressed(J_A)) {
      load_move_names();
      battle_state = BATTLE_STATE_FIGHT;
      move_cursor(BATTLE_CURSOR_MOVE1);
    }
    break;
  case BATTLE_CURSOR_SKILL:
    if (was_pressed(J_RIGHT))
      move_cursor(BATTLE_CURSOR_RUN);
    else if (was_pressed(J_UP))
      move_cursor(BATTLE_CURSOR_FIGHT);
    else if (was_pressed(J_A)) {
      load_skill_names();
      battle_state = BATTLE_STATE_SKILL;
      move_cursor(BATTLE_CURSOR_MOVE1);
    }
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
  const uint8_t move_idx = battle_cursor - BATTLE_CURSOR_MOVE1;

  switch (battle_state) {
  case BATTLE_STATE_MAIN_MENU:
    handle_main_menu();
    break;
  case BATTLE_STATE_FIGHT:
    MonsterMove *next_move = battle_monster_moves + move_idx + 1;

    if (was_pressed(J_UP) && battle_cursor > BATTLE_CURSOR_MOVE1)
      move_cursor(battle_cursor - 1);
    else if (
      was_pressed(J_DOWN) &&
      battle_cursor < BATTLE_CURSOR_MOVE4 &&
      next_move->id != 0
    )
      move_cursor(battle_cursor + 1);
    else if (was_pressed(J_B)) {
      battle_state = BATTLE_STATE_MAIN_MENU;
      move_cursor(BATTLE_CURSOR_FIGHT);
    }
    break;
  case BATTLE_STATE_SKILL:
    PlayerSkill *next_skill = battle_player_skills + move_idx + 1;

    if (was_pressed(J_UP) && battle_cursor > BATTLE_CURSOR_MOVE1)
      move_cursor(battle_cursor - 1);
    else if (
      was_pressed(J_DOWN) &&
      battle_cursor < BATTLE_CURSOR_MOVE4 &&
      next_skill->id != 0
    )
      move_cursor(battle_cursor + 1);
    else if (was_pressed(J_B)) {
      battle_state = BATTLE_STATE_MAIN_MENU;
      move_cursor(BATTLE_CURSOR_FIGHT);
    }
    break;
  // case BATTLE_STATE_TEXT:
  //   break;
  }
}

void draw_battle(void) {
  move_bkg(0, 0);
}
