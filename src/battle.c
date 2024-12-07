#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "battle.h"
#include "bcd.h"
#include "data.h"
#include "joypad.h"
#include "monster.h"
#include "palette.h"
#include "util.h"

// -----------------------------------------------------------------------------
// TODO: Abstract me out of here / finish up when done
// -----------------------------------------------------------------------------

typedef enum AbilityType {
  ABILITY_TYPE_FIGHT,
  ABILITY_TYPE_MAGIC,
  ABILITY_TYPE_TECH,
} AbilityType;

typedef struct Ability {
  uint8_t id;
  char name[12];
} Ability;

typedef struct Summon {
  uint8_t id;
  char name[12];
} Summon;

/**
 * Null or empty monster.
 */
const Monster MONSTER_NULL = { 0 };

/**
 * Palette colors for the beholder monster.
 */
const palette_color_t MONSTER_BEHOLDER_PALETTE[4] = {
  RGB_WHITE,
  RGB8(209, 206, 107),
  RGB8(126, 73, 73),
  RGB8(0, 40, 51),
};

/**
 * Monster stats and data for the "Beholder".
 */
const Monster MONSTER_BEHOLDER = {
  // id, name
  1, "BEHOLDER",
  // tile_bank, tile_data, palette
  3, tile_beholder, MONSTER_BEHOLDER_PALETTE,
  // Max HP & MP
  115, 50,
  // Attack, AC, Magic Attack, Saving Throw, Initiative
  0, 0, 0, 0, 0
};

/**
 * Palette Colors for the Kobold.
 */
const palette_color_t MONSTER_KOBOLD_PALETTE[4] = {
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
};

/**
 * Monster stats and data for the "Beholder".
 */
const Monster MONSTER_KOBOLD = {
  // id, name
  1, "KOBOLD",
  // tile_bank, tile_data, palette
  3, tile_kobold, MONSTER_KOBOLD_PALETTE,
  // Max HP & MP
  15, 10,
  // Attack, AC, Magic Attack, Saving Throw, Initiative
  0, 0, 0, 0, 0
};

// -----------------------------------------------------------------------------


BattleState battle_state;
BattleMenu battle_menu;
BattleCursor battle_cursor;

uint8_t battle_num_monsters;
MonsterInstance battle_monsters[] = { {0}, {0}, {0} };

/**
 * Default palettes for the battle system.
 */
palette_color_t battle_bg_palettes[] = {
  // Palette 0 - Background / Textbox
  RGB_WHITE,
  RGB8(111, 127, 243),
  RGB8(54, 112, 54),
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
  RGB_WHITE,
  RGB8(40, 150, 40),
  RGB8(120, 120, 120),
  RGB_BLACK,
  // Palette 7 - Debuff
  RGB_WHITE,
  RGB8(150, 40, 40),
  RGB8(120, 120, 120),
  RGB_BLACK,
};

/**
 * LCY interrupt handler for the fight and skill menus. This handler changes the
 * scroll-y position at a specific scanline to display a different part of the
 * background that contains the graphics for these menus.
 */
void fight_menu_isr(void) {
  SCY_REG = 0;
  if (battle_state == BATTLE_STATE_MENU && battle_menu != BATTLE_MENU_MAIN) {
    // TODO: Fix me
    SCY_REG = 80;
    return;
  }
}

/**
 * Draws the tiles for the given monster layout.
 * @param layout Monster layout to draw to the background.
 */
void draw_monster_layout(MonsterLayout layout) {
  const uint16_t offset = 20 * 11 * 2 * (uint16_t)layout;
  const uint8_t *src = tilemap_battle_monster_layouts + offset;
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint8_t y = 0; y < 11; y++, vram += (32 - 20)) {
    for (uint8_t x = 0; x < 20; x++, vram++) {
      VBK_REG = VBK_TILES;
      *vram = *src++;
      VBK_REG = VBK_ATTRIBUTES;
      *vram = *src++;
    }
  }
}

/**
 * Loads an new instance of the given monster into the given position.
 * @param p Position of the monsters on the battle screen.
 * @param monster Pointer to the monster to load.
 */
void load_monster(MonsterPosition p, Monster *monster) {
  battle_monsters[p] = monster_instance((uint8_t)p, monster);

  const uint8_t total_tiles = 2 * 7 * 7;
  const uint8_t *src = monster->tile_data;
  uint16_t page_offset = total_tiles * 16;

  switch (p) {
  case MONSTER_POSITION1:
    VBK_REG = VBK_BANK_0;
    load_tiles(
      monster->tile_bank,
      monster->tile_data,
      (void *)0x9000,
      total_tiles
    );
    update_bg_palettes(1, 1, monster->palette);
    break;
  case MONSTER_POSITION2:
    VBK_REG = VBK_BANK_0;
    // 30 -> VRAM_BG_TILES + page_offset
    load_tiles(
      monster->tile_bank,
      monster->tile_data,
      (void *)0x9620,
      30
    );
    // 68 -> VRAM_SHARED_TILES
    load_tiles(
      monster->tile_bank,
      monster->tile_data + 30 * 16,
      (void *)0x8800,
      68
    );
    update_bg_palettes(2, 1, monster->palette);
    break;
  case MONSTER_POSITION3:
    // 60 -> VRAM_SHARED_TILES + 68 * 16
    VBK_REG = VBK_BANK_0;
    load_tiles(
      monster->tile_bank,
      monster->tile_data,
      (void *)0x8C40,
      60
    );
    // 38 -> VRAM_BG_TILES BANK[2]
    VBK_REG = VBK_BANK_1;
    load_tiles(
      monster->tile_bank,
      monster->tile_data + 60 * 16,
      (void *)0x9000,
      38
    );
    update_bg_palettes(3, 1, monster->palette);
    break;
  }
}

/**
 * Draws one of the three major battle menus to the given address in VRAM.
 * The battle maps are drawn using a single tilemap that contains all three.
 * @param layout The battle menu layout to draw.
 * @param vram Pointer in VRAM to write the menu tiles.
 */
void draw_battle_menu(BattleMenuLayout layout, uint8_t *vram) {
  const uint8_t w = 20;
  const uint8_t h = 7;
  const uint8_t *src = tilemap_battle_menus;

  switch (layout) {
  case BATTLE_MENU_LAYOUT_MAIN:
    break;
  case BATTLE_MENU_LAYOUT_SUBMENU:
    src = tilemap_battle_menus + w * 7 * 2;
    break;
  case BATTLE_MENU_LAYOUT_TEXT:
    src = tilemap_battle_menus + w * 14 * 2;
    break;
  }

  for (uint8_t y = 0; y < h; y++, vram += 0x20 - w) {
    for (uint8_t x = 0; x < w; x++, vram++) {
      VBK_REG = VBK_TILES;
      *vram = *src++;
      VBK_REG = VBK_ATTRIBUTES;
      *vram = *src++;
    }
  }
}

/**
 * Moves the battle cursor to the given cursor position.
 */
void move_cursor(BattleCursor c) {
  battle_cursor = c;
}

void init_battle(void) {
  lcd_off();

  // Reset the background and window position
  fill_background(0x30, 0x10);
  move_win(0, 144);

  // Load Palettes for the battle system
  update_bg_palettes(0, 8, battle_bg_palettes);
  // TODO Flesh out sprite palettes for effects, damage numbers, etc.
  update_sprite_palettes(0, 1, battle_bg_palettes);

  // Draw the monster layout and instantiate the monsters for the fight
  // TODO Load this data based on the fight type
  draw_monster_layout(MONSTER_LAYOUT_2);
  load_monster(MONSTER_POSITION1, &MONSTER_BEHOLDER);
  load_monster(MONSTER_POSITION2, &MONSTER_KOBOLD);

  // Load the font and battle specific tilesets
  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_font, VRAM_SHARED_TILES);
  load_tiles(1, tile_battle, (void *)(0x9300), 5 * 16);

  // Draw the battle menus
  draw_battle_menu(BATTLE_MENU_LAYOUT_MAIN, VRAM_BACKGROUND_XY(0, 11));
  draw_battle_menu(BATTLE_MENU_LAYOUT_SUBMENU, VRAM_BACKGROUND_XY(0, 18));
  draw_battle_menu(BATTLE_MENU_LAYOUT_TEXT, VRAM_WINDOW_XY(0, 0));

  // Setup cursor sprite
  set_sprite_tile(0, 0xFE);
  set_sprite_prop(SPRITE_CURSOR, 0x0F);
  move_cursor(BATTLE_CURSOR_MAIN_FIGHT);

  // TODO Load player & skills, and monsters

  // Attach an LCY=LY interrupt to handle the menu display.
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

void update_battle(void) {
}

void draw_battle(void) {
  move_bkg(0, 0);
}
