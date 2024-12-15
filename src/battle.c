#pragma bank 3

#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "battle.h"
#include "battle_text.h"
#include "bcd.h"
#include "data.h"
#include "encounter.h"
#include "joypad.h"
#include "monster.h"
#include "palette.h"
#include "strings.h"
#include "util.h"

BattleState battle_state;
BattleMenu battle_menu;
BattleCursor battle_cursor;
uint8_t battle_num_submenu_items;

Timer status_effect_timer;
uint8_t status_effect_frame = 0;

BattleAnimation battle_animation;
char battle_pre_message[64];
char battle_post_message[64];

/**
 * Determines the x position for the HP bar based on the given monster position
 * and the current monster layout.
 * @param pos Position of the monster on the screen.
 * @return The x position for the HP bar.
 */
inline uint8_t get_hp_bar_x(MonsterPosition pos) {
  switch (encounter.layout) {
  case MONSTER_LAYOUT_1:
    // 1 Monster  - (6, 9)
    return 6;
  case MONSTER_LAYOUT_2:
    // 2 Monsters - (2, 9), (11, 9)
    return (pos == MONSTER_POSITION1) ? 2 : 11;
  case MONSTER_LAYOUT_3S:
    // 3 Monsters - (0, 9), (6, 9), (12, 9)
    switch (pos) {
    case MONSTER_POSITION1: return 0;
    case MONSTER_POSITION2: return 6;
    case MONSTER_POSITION3: return 12;
    }
  case MONSTER_LAYOUT_1M_2S:
    // 1 md + 2 sm - (0) (7) (13)
    switch (pos) {
    case MONSTER_POSITION1: return 0;
    case MONSTER_POSITION2: return 7;
    case MONSTER_POSITION3: return 13;
    }
  }
  return 6;
}

/**
 * Determines the starting x position for a monster's status effect tiles.
 * @param pos Monster position.
 * @return The starting x position for the status effects.
 */
inline uint8_t get_status_effect_x(MonsterPosition pos) {
  switch (encounter.layout) {
  case MONSTER_LAYOUT_1: return 7;
  case MONSTER_LAYOUT_2: return (pos == MONSTER_POSITION1) ? 3 : 12;
  case MONSTER_LAYOUT_3S:
    switch (pos) {
    case MONSTER_POSITION1: return 1;
    case MONSTER_POSITION2: return 7;
    case MONSTER_POSITION3: return 13;
    }
  case MONSTER_LAYOUT_1M_2S:
    switch (pos) {
    case MONSTER_POSITION1: return 1;
    case MONSTER_POSITION2: return 8;
    case MONSTER_POSITION3: return 14;
    }
  }
  return 7;
}

/**
 * @return `true` If the submenu contains a number of selectable items.
 */
inline bool has_submenu_items(void) {
  return battle_num_submenu_items > 0;
}

/**
 * Shows the battle text box.
 */
inline void show_battle_text(void) {
  move_win(7, 88);
}

/**
 * Hides the battle textbox.
 */
inline void hide_battle_text(void) {
  move_win(0, 144);
}

/**
 * Sets and draws the monster layout.
 * @param layout Monster layout to draw to the background.
 */
void set_monster_layout(MonsterLayout layout) {
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
 * Loads the graphics and palettes for a monster instance.
 * @param p Position of the monsters on the battle screen.
 * @param m Monster instance to load.
 */
void load_monster_graphics(MonsterPosition p, MonsterInstance *m) {
  if (!m->active || !m->monster)
    return;

  const uint8_t total_tiles = 2 * 7 * 7;
  const uint8_t *src = m->monster->tile_data;
  uint16_t page_offset = total_tiles * 16;

  switch (p) {
  case MONSTER_POSITION1:
    VBK_REG = VBK_BANK_0;
    load_tiles(
      m->monster->tile_bank,
      m->monster->tile_data,
      (void *)0x9000,
      total_tiles
    );
    update_bg_palettes(1, 1, m->palette);
    break;
  case MONSTER_POSITION2:
    VBK_REG = VBK_BANK_0;
    // 30 -> VRAM_BG_TILES + page_offset
    load_tiles(
      m->monster->tile_bank,
      m->monster->tile_data,
      (void *)0x9620,
      30
    );
    // 68 -> VRAM_SHARED_TILES
    load_tiles(
      m->monster->tile_bank,
      m->monster->tile_data + 30 * 16,
      (void *)0x8800,
      68
    );
    update_bg_palettes(2, 1, m->palette);
    break;
  case MONSTER_POSITION3:
    // 60 -> VRAM_SHARED_TILES + 68 * 16
    VBK_REG = VBK_BANK_0;
    load_tiles(
      m->monster->tile_bank,
      m->monster->tile_data,
      (void *)0x8C40,
      60
    );
    // 38 -> VRAM_BG_TILES BANK[2]
    VBK_REG = VBK_BANK_1;
    load_tiles(
      m->monster->tile_bank,
      m->monster->tile_data + 60 * 16,
      (void *)0x9000,
      38
    );
    update_bg_palettes(3, 1, m->palette);
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
 * Removes a monster HP bar from the screen.
 */
void remove_hp_bar(MonsterPosition pos) {
  const uint8_t x = get_hp_bar_x(pos);
  const uint8_t y = 9;
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);
  VBK_REG = VBK_TILES;
  for (uint8_t k = 0; k < 7; k++, vram++) {
    set_vram_byte(vram, BATTLE_CLEAR_TILE);
  }
}

/**
 * Draws an HP bar for the monster at a given position.
 * @param pos Position of the monster on the battle screen.
 * @param hp Curent HP for the monster.
 * @param max Max HP for the monster.
 */
void draw_hp_bar(MonsterPosition pos, uint8_t hp, uint8_t max) {
  // Determine the x, y position for the bar
  const uint8_t x = get_hp_bar_x(pos);
  const uint8_t y = 9;
  uint8_t col = 0;
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);

  // Calculate the number of pips based on the ratio of HP to Max HP
  uint16_t k = (40 * hp) / max;
  uint8_t p = (uint8_t)k;

  // Update the attributes based on HP percentage
  // If p <= 13, then HP <= (1/3) * max, so set critical palette
  VBK_REG = VBK_ATTRIBUTES;
  for (col = 0; col < 7; col++)
    set_vram_byte(vram++, p <= 13 ? 0b00001101 : 0b00001100);

  // Draw the graphics
  vram -= 7;
  VBK_REG = VBK_TILES;
  col = 0;

  // Draw the "left cap"
  set_vram_byte(vram++, HP_BAR_LEFT_CAP);

  // Fill full bar sprites until the pips value falls below 8
  while (p >= 8) {
    set_vram_byte(vram++, HP_BAR_FULL_PIPS);
    p -= 8;
    col++;
  }

  // If we've not filled all the pip sprites, then draw the next based on the
  // remaining pips
  if (col < 5) {
    set_vram_byte(vram++, HP_BAR_EMPTY_PIPS + p);
    col++;
  }

  // Draw any remaining pip sprites as "empty"
  while (col < 5) {
    set_vram_byte(vram++, HP_BAR_EMPTY_PIPS);
    col++;
  }

  // Finally draw the "right cap"
  set_vram_byte(vram, HP_BAR_RIGHT_CAP);
}

/**
 * Removes status effect graphics for a monster on the screen.
 * @param pos Position of the monster.
 */
void remove_status_effects(MonsterPosition pos) {
  const uint8_t x = get_status_effect_x(pos);
  const uint8_t y = 8;
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);
  for (uint8_t k = 0; k < 5; k++, vram++) {
    VBK_REG = VBK_TILES;
    set_vram_byte(vram, BATTLE_CLEAR_TILE);
    VBK_REG = VBK_ATTRIBUTES;
    set_vram_byte(vram, BATTLE_CLEAR_ATTR);
  }
}

/**
 * Draws the icon for a status effect at the given position for a monster on the
 * screen.
 * @param e Status effect to draw.
 * @param m Position of the monster with the effect.
 * @param p Position index.
 */
void draw_monster_status_effect_icon(
  StatusEffect e,
  MonsterPosition m,
  uint8_t p
) {
  uint8_t tile = ((uint8_t)e * 2) + 0x60 + status_effect_frame;
  uint8_t attr = e < 8 ? DEBUFF_ATTRIBUTE : BUFF_ATTRIBUTE;
  const uint8_t x = get_status_effect_x(m);
  const uint8_t y = 8;
  uint8_t *vram = VRAM_BACKGROUND_XY(x + p, y);
  VBK_REG = VBK_TILES;
  set_vram_byte(vram, tile);
  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram, attr);
}

/**
 * Draws the icon for a player status effect at the given position.
 */
void draw_player_status_effect_icon(StatusEffect e, uint8_t p) {
  uint8_t tile = ((uint8_t)e * 2) + 0x60 + status_effect_frame;
  const uint8_t x = 10 + p;
  const uint8_t y = 13;
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);
  VBK_REG = VBK_TILES;
  set_vram_byte(vram, tile);
}

/**
 * Redraws all status effects for the player.
 */
void redraw_player_status_effects(void) {
  // TODO Implement me
  // draw_player_status_effect_icon(BUFF_ATK_UP, 0);
  // draw_player_status_effect_icon(DEBUFF_PARALZYED, 1);
}

/**
 * Draws status effects for all monsters and the player.
 */
void redraw_monster_status_effects(void) {
  // TODO Implement me
  // draw_monster_status_effect_icon(BUFF_DEF_UP, MONSTER_POSITION1, 0);
  // draw_monster_status_effect_icon(DEBUFF_POISONED, MONSTER_POSITION1, 1);
}

// TODO document me
void print_fraction(uint8_t *vram, uint16_t n, uint16_t d) {
  bcd_t left, right;
  to_bcd16(n, &left);
  to_bcd16(d, &right);

  VBK_REG = VBK_TILES;

  if (left.d2) {
    set_vram_byte(vram++, left.d2 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, left.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, left.d0 + FONT_DIGIT_OFFSET);
  } else if (!left.d2 && left.d1) {
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, left.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, left.d0 + FONT_DIGIT_OFFSET);
  } else {
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, left.d0 + FONT_DIGIT_OFFSET);
  }

  set_vram_byte(vram++, FONT_SLASH);

  if (right.d2) {
    set_vram_byte(vram++, right.d2 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, right.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, right.d0 + FONT_DIGIT_OFFSET);
  } else if (!right.d2 && right.d1) {
    set_vram_byte(vram++, right.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, right.d0 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, FONT_SPACE);
  } else {
    set_vram_byte(vram++, right.d0 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, FONT_SPACE);
  }
}

/**
 * Sets the battle system to use the magic or tech menu based on the player
 * character's chosen class.
 */
void set_magic_or_tech_menu(void) {
  uint8_t menu_icon, resource_icon;
  if (is_magic_class()) {
    menu_icon = MAGIC_ICON;
    resource_icon = MP_ICON_LEFT;
  } else {
    menu_icon = TECH_ICON;
    resource_icon = SP_ICON_LEFT;
  }
  uint8_t *vram = VRAM_BACKGROUND_XY(2, MENU_Y + 2);
  VBK_REG = VBK_TILES;
  for (uint8_t k = 0; k < 4; k++)
    set_vram_byte(vram++, menu_icon + k);
  vram = VRAM_BACKGROUND_XY(9, MENU_Y + 4);
  set_vram_byte(vram++, resource_icon);
  set_vram_byte(vram, resource_icon + 1);
}

/**
 * Moves cursor sprites offscreen to hide them.
 */
void hide_cursor_sprites(void) {
  move_sprite(CURSOR_SPRITE + 0, 0, 0);
  move_sprite(CURSOR_SPRITE + 1, 0, 0);
  move_sprite(CURSOR_SPRITE + 2, 0, 0);
  move_sprite(CURSOR_SPRITE + 3, 0, 0);
}

/**
 * Moves the cursor sprites to the given column and row on the screen.
 * @param col Column for the first sprite.
 * @param row Row for the first sprite.
 */
void move_cursor_sprites(uint8_t col, uint8_t row) {
  uint8_t x = (col + 1) << 3;
  uint8_t y = ((row + 2) << 3) - 1;
  move_sprite(CURSOR_SPRITE + 0, x, y);
  move_sprite(CURSOR_SPRITE + 1, x + 8, y);
  move_sprite(CURSOR_SPRITE + 2, x, y + 8);
  move_sprite(CURSOR_SPRITE + 3, x + 8, y + 8);
}

/**
 * Moves the battle cursor to the given cursor position.
 * @param c Cursor position to set.
 */
void move_cursor(BattleCursor c) {
  battle_cursor = c;

  switch (battle_cursor) {
  case BATTLE_CURSOR_MAIN_FIGHT:
    move_cursor_sprites(0, 12);
    break;
  case BATTLE_CURSOR_MAIN_ABILITY:
    move_cursor_sprites(0, 13);
    break;
  case BATTLE_CURSOR_MAIN_ITEM:
    move_cursor_sprites(0, 14);
    break;
  case BATTLE_CURSOR_MAIN_MAIN_SUMMON:
    move_cursor_sprites(0, 15);
    break;
  case BATTLE_CURSOR_MAIN_FLEE:
    move_cursor_sprites(0, 16);
    break;
  case BATTLE_CURSOR_ITEM_1:
    move_cursor_sprites(0, 12);
    break;
  case BATTLE_CURSOR_ITEM_2:
    move_cursor_sprites(0, 13);
    break;
  case BATTLE_CURSOR_ITEM_3:
    move_cursor_sprites(0, 14);
    break;
  case BATTLE_CURSOR_ITEM_4:
    move_cursor_sprites(0, 15);
    break;
  case BATTLE_CURSOR_ITEM_5:
    move_cursor_sprites(0, 16);
    break;
  case BATTLE_CURSOR_NO_ITEMS:
    hide_cursor_sprites();
  case BATTLE_CURSOR_MONSTER_1:
    switch (encounter.layout) {
    case MONSTER_LAYOUT_1:
      move_cursor_sprites(5, 5);
      break;
    case MONSTER_LAYOUT_2:
      move_cursor_sprites(1, 5);
      break;
    case MONSTER_LAYOUT_3S:
    case MONSTER_LAYOUT_1M_2S:
      move_cursor_sprites(0, 5);
      break;
    }
    break;
  case BATTLE_CURSOR_MONSTER_2:
    switch (encounter.layout) {
    case MONSTER_LAYOUT_1:
      move_cursor_sprites(5, 5);
      break;
    case MONSTER_LAYOUT_2:
      move_cursor_sprites(10, 5);
      break;
    case MONSTER_LAYOUT_3S:
      move_cursor_sprites(6, 5);
      break;
    case MONSTER_LAYOUT_1M_2S:
      move_cursor_sprites(7, 5);
      break;
    }
    break;
  case BATTLE_CURSOR_MONSTER_3:
    switch (encounter.layout) {
    case MONSTER_LAYOUT_1:
      move_cursor_sprites(5, 5);
      break;
    case MONSTER_LAYOUT_2:
      move_cursor_sprites(10, 5);
      break;
    case MONSTER_LAYOUT_3S:
      move_cursor_sprites(12, 5);
      break;
    case MONSTER_LAYOUT_1M_2S:
      move_cursor_sprites(13, 5);
      break;
    }
    break;
  }
}

/**
 * Initializes the cursor and its sprites.
 */
void init_cursor(void) {
  for (uint8_t s = 0; s < 4; s++) {
    set_sprite_tile(CURSOR_SPRITE + s, CURSOR_TILE + s);
    set_sprite_prop(CURSOR_SPRITE + s, CURSOR_ATTR);
  }
  move_cursor(BATTLE_CURSOR_MAIN_FIGHT);
}

/**
 * LCY interrupt handler for the fight and skill menus. This handler changes the
 * scroll-y position at a specific scanline to display a different part of the
 * background that contains the graphics for these menus.
 */
void fight_menu_isr(void) {
  SCY_REG = 0;
  if (
    battle_state == BATTLE_STATE_MENU &&
    battle_menu != BATTLE_MENU_MAIN &&
    battle_menu != BATTLE_MENU_FIGHT
  ) {
    SCY_REG = 56;
    return;
  }
}

/**
 * Initializes graphics and state for the encounter and monsters.
 */
void battle_init_encounter(void) {
  // The encounter should already be generate prior to this point
  // TODO Remove this after testing
  encounter.layout = MONSTER_LAYOUT_2;
  MonsterInstance *monster = encounter.monsters;

  kobold_generator(monster, 2, C_TIER);
  monster->id = 'A';

  kobold_generator(++monster, 3, B_TIER);
  monster->id = 'B';

  monster_deactivate(++monster);

  // Initialize graphics for the monsters in the encounter
  set_monster_layout(encounter.layout);
  monster = encounter.monsters;

  load_monster_graphics(MONSTER_POSITION1, monster++);
  load_monster_graphics(MONSTER_POSITION2, monster++);
  load_monster_graphics(MONSTER_POSITION3, monster);
}

/**
 * Initializes graphics and state for the player.
 */
void battle_init_player(void) {
  player.summon->activate();
  draw_text(VRAM_SUMMON_NAME, player.summon->name, 9);
  set_magic_or_tech_menu();

  // TODO The player should already be initialized, remove this after testing
  init_player();

  print_fraction(VRAM_BACKGROUND_XY(12, 14), player.hp, player.max_hp);
  print_fraction(VRAM_BACKGROUND_XY(12, 15), player.sp, player.max_sp);
}

void init_battle(void) NONBANKED {
  lcd_off();

  // Switch to the battle ROM bank (has stats tables, etc.)
  SWITCH_ROM(BATTLE_ROM_BANK);

  // Reset the background and window position
  fill_background(BATTLE_CLEAR_TILE, BATTLE_CLEAR_ATTR);
  move_win(0, 144);

  // Load palettes & fonts
  update_bg_palettes(0, 8, battle_bg_palettes);
  update_sprite_palettes(0, 8, battle_sprite_palettes);
  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_font, VRAM_SHARED_TILES);
  load_tiles(1, tile_battle, (void *)(0x9300), 5 * 16);

  // Draw menus and initialize cursor
  draw_battle_menu(BATTLE_MENU_LAYOUT_MAIN, VRAM_BACKGROUND_XY(0, MENU_Y));
  draw_battle_menu(BATTLE_MENU_LAYOUT_SUBMENU, VRAM_BACKGROUND_XY(0, SUBMENU_Y));
  draw_battle_menu(BATTLE_MENU_LAYOUT_TEXT, VRAM_WINDOW_XY(0, 0));
  init_cursor();

  // Initialize the encounter and player graphics
  battle_init_encounter();
  battle_init_player();

  // Attach an LCY=LY interrupt to handle the menu display.
  CRITICAL {
    LYC_REG = 87;
    STAT_REG = STATF_LYC;
    add_LCD(fight_menu_isr);
    add_LCD(nowait_int_handler);
  }
  set_interrupts(IE_REG | LCD_IFLAG);

  // Animation & timers
  init_timer(status_effect_timer, 40);

  lcd_on();
}

void cleanup_battle(void) {
  CRITICAL {
    remove_LCD(fight_menu_isr);
    remove_LCD(nowait_int_handler);
    STAT_REG = 0;
  }
  set_interrupts(IE_REG & ~LCD_IFLAG);
}



/**
 * Confirms that the player has chosen the default "fight" action and begins the
 * next round of combat.
 */
void confirm_fight(void) {
  uint8_t monster_idx = battle_cursor - BATTLE_CURSOR_MONSTER_1;
  MonsterInstance *target = get_monster(monster_idx);
  set_player_fight(target);
  battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms that the player has chosen an ability action and begins the next
 * round of combat.
 */
void confirm_ability(void) {
  // battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms that the player has chosen to use an item and begins the next round
 * of combat.
 */
void confirm_item(void) {
  // battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms the player has chosen a summon and begins the next round of combat.
 */
void confirm_summon(void) {
  // battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms that the player has selected the "flee" action from the main menu
 * and begins the next round of combat.
 */
void confirm_flee(void) {
}

/**
 * Moves the cursor to select the previous enemy in the list.
 */
void select_prev_enemy(void) {
  switch (encounter.layout) {
  case MONSTER_LAYOUT_1:
    return;
  case MONSTER_LAYOUT_2:
    if (battle_cursor == BATTLE_CURSOR_MONSTER_1)
      move_cursor(BATTLE_CURSOR_MONSTER_2);
    else
      move_cursor(BATTLE_CURSOR_MONSTER_1);
    break;
  case MONSTER_LAYOUT_3S:
  case MONSTER_LAYOUT_1M_2S:
    if (battle_cursor == BATTLE_CURSOR_MONSTER_1)
      move_cursor(BATTLE_CURSOR_MONSTER_3);
    else
      move_cursor(battle_cursor - 1);
    break;
  }
}

/**
 * Moves the cursor to select the next enemy in the list.
 */
void select_next_enemy(void) {
  switch (encounter.layout) {
  case MONSTER_LAYOUT_1:
    return;
  case MONSTER_LAYOUT_2:
    if (battle_cursor == BATTLE_CURSOR_MONSTER_1)
      move_cursor(BATTLE_CURSOR_MONSTER_2);
    else
      move_cursor(BATTLE_CURSOR_MONSTER_1);
    break;
  case MONSTER_LAYOUT_3S:
  case MONSTER_LAYOUT_1M_2S:
    if (battle_cursor == BATTLE_CURSOR_MONSTER_3)
      move_cursor(BATTLE_CURSOR_MONSTER_1);
    else
      move_cursor(battle_cursor + 1);
    break;
  }
}

/**
 * Draws the "EMPTY..." message in the middle of an empty submenu.
 */
inline void draw_menu_empty_text(void) {
  draw_text(VRAM_BACKGROUND_XY(7, 21), str_misc_empty, 6);
}

/**
 * Draws the submenu heading based on which battle menu is currently set.
 */
inline void draw_submenu_heading(void) {
  uint8_t start_tile;
  uint8_t len;
  switch (battle_menu) {
  case BATTLE_MENU_ABILITY:
    start_tile = is_magic_class() ? MAGIC_ICON : TECH_ICON;
    len = 4;
    break;
  case BATTLE_MENU_ITEM:
    start_tile = ITEM_ICON;
    len = 4;
    break;
  default:
    start_tile = SUMMON_ICON;
    len = 5;
  }
  uint8_t *vram = VRAM_BACKGROUND_XY(1, 18);
  uint8_t k;
  for (k = 0; k < len; k++, start_tile++, vram++)
    set_vram_byte(vram, start_tile);
  while (k++ < 5)
    set_vram_byte(vram++, FONT_BORDER_TOP);
}

/**
 * Clears the body of the submenu.
 */
inline void clear_submenu_body(void) {
  uint8_t *vram = VRAM_BACKGROUND_XY(SUBMENU_TEXT_X - 1, SUBMENU_TEXT_Y);
  for (uint8_t y = 0; y < 5; y++) {
    for (uint8_t x = 0; x < 18; x++)
      set_vram_byte(vram++, FONT_SPACE);
    vram += 14;
  }
}

/**
 * Resets the battle submenu graphics and state.
 */
void reset_battle_submenu(void) {
  battle_num_submenu_items = 0;
  hide_cursor_sprites();
  draw_submenu_heading();
  clear_submenu_body();
}

/**
 * Draws a list of selectable abilities into the battle submenu.
 */
void load_abilities_menu(void) {
  Ability *ability = player.summon->ability;
  if (!ability) {
    draw_menu_empty_text();
    return;
  }

  battle_num_submenu_items = 0;
  uint8_t *vram = VRAM_SUBMENU_TEXT;
  while (ability && player.level >= ability->level) {
    draw_text(vram, ability->name, 14);
    uint8_t *cost_tile = ability->sp_cost_tiles;
    set_vram_byte(vram + 13, *cost_tile++);
    set_vram_byte(vram + 14, *cost_tile++);
    set_vram_byte(vram + 15, *cost_tile++);
    vram += 32;
    ability = ability->next;
    battle_num_submenu_items++;
  }

  move_cursor(BATTLE_CURSOR_ITEM_1);
}

/**
 * Draws a list of selectable items into the battle submenu.
 */
void load_items_menu(void) {
  // TODO Load items names and state
  draw_menu_empty_text();
}

/**
 * Draws a list of selectable summons into the battle submenu.
 */
void load_summons_menu(void) {
  // TODO Load summons names and state
  draw_menu_empty_text();
}

/**
 * Opens the given battle menu and updates the battle menu state.
 * @param m Battle menu to open.
 */
void open_battle_menu(BattleMenu m) {
  uint8_t prev_menu = battle_menu;
  battle_menu = m;
  switch (battle_menu) {
  case BATTLE_MENU_MAIN:
    move_cursor(prev_menu - 1);
    break;
  case BATTLE_MENU_FIGHT:
    move_cursor(BATTLE_CURSOR_MONSTER_1);
    break;
  case BATTLE_MENU_ABILITY:
    reset_battle_submenu();
    load_abilities_menu();
    break;
  case BATTLE_MENU_ITEM:
    reset_battle_submenu();
    load_items_menu();
    break;
  case BATTLE_MENU_SUMMON:
    reset_battle_submenu();
    load_summons_menu();
    break;
  }
}

/*
 * Moves the battle cursor for submenu item selection according to the current
 * number items available.
 */
void move_submenu_item_by_dpad(void) {
  if (!has_submenu_items())
    return;
  const uint8_t min = BATTLE_CURSOR_ITEM_1;
  const uint8_t max = BATTLE_CURSOR_ITEM_1 + battle_num_submenu_items - 1;
  if (was_pressed(J_UP) && battle_cursor > min)
    move_cursor(battle_cursor - 1);
  else if (was_pressed(J_DOWN) && battle_cursor < max)
    move_cursor(battle_cursor + 1);
}

/**
 * Performs game logic for the battle menu.
 * @see `update_battle`
 */
inline void update_battle_menu(void) {
  switch (battle_menu) {
  case BATTLE_MENU_MAIN:
    if (was_pressed(J_UP) && battle_cursor != BATTLE_CURSOR_MAIN_FIGHT)
      move_cursor(battle_cursor - 1);
    else if (was_pressed(J_DOWN) && battle_cursor != BATTLE_CURSOR_MAIN_FLEE)
      move_cursor(battle_cursor + 1);
    else if (was_pressed(J_A) && battle_cursor != BATTLE_CURSOR_MAIN_FLEE) {
      open_battle_menu(battle_cursor + 1);
    } else if (was_pressed(J_A)) {
      confirm_flee();
    }
    break;
  case BATTLE_MENU_FIGHT:
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    else if (was_pressed(J_A))
      confirm_fight();
    else if (was_pressed(J_LEFT))
      select_prev_enemy();
    else if (was_pressed(J_RIGHT))
      select_next_enemy();
    break;
  case BATTLE_MENU_ABILITY:
    // Select an available tech
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    else if (was_pressed(J_A) && has_submenu_items())
      confirm_ability();
    else
      move_submenu_item_by_dpad();
    break;
  case BATTLE_MENU_ITEM:
    // Select an item from the inventory
    // TODO Create inventory/item system xD
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    break;
  case BATTLE_MENU_SUMMON:
    // Select an available summon
    // TODO Create more than one summon to build this out
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    break;
  }
}

//------------------------------------------------------------------------------
// TODO Organize animation code elsewhere
//------------------------------------------------------------------------------

typedef enum AnimationState {
  ANIMATION_PREAMBLE,
  ANIMATION_EFFECT,
  ANIMATION_RESULT,
  ANIMATION_COMPLETE,
} AnimationState;

#define ANIMATION_HP_DELTA_FACTOR 4

AnimationState animation_state = ANIMATION_PREAMBLE;
Timer effect_delay_timer;

inline uint8_t tween_hp(uint16_t hp, uint16_t target) {
  uint16_t delta;
  if (hp < target) {
    delta = (target - hp) >> ANIMATION_HP_DELTA_FACTOR;
    if (delta == 0)
      delta = 1;
    hp += delta;
    if (hp > target)
      hp = target;
  } else if (hp > target) {
    delta = (hp - target) >> ANIMATION_HP_DELTA_FACTOR;
    if (delta == 0)
      delta = 1;
    hp -= delta;
    if (hp < target)
      hp = target;
  }
  return hp;
}

inline bool animate_monster_hp_bars(void) {
  bool updated = false;
  MonsterInstance *monster = encounter.monsters;
  for (uint8_t pos = 0; pos < 3; pos++, monster++) {
    if (!monster->active || monster->hp == monster->target_hp)
      continue;
    updated = true;
    monster->hp = tween_hp(monster->hp, monster->target_hp);
    draw_hp_bar(pos, monster->hp, monster->max_hp);
  }
  return updated;
}

void animate_action_result(void) {
  switch (animation_state) {
  case ANIMATION_PREAMBLE:
    if (battle_text.state == BATTLE_TEXT_DONE) {
      init_timer(effect_delay_timer, 30);
      animation_state = ANIMATION_EFFECT;
    }
    break;
  case ANIMATION_EFFECT:
    if (update_timer(effect_delay_timer)) {
      battle_text.print(battle_post_message);
      animation_state = ANIMATION_RESULT;
    }
    break;
  case ANIMATION_RESULT:
    // Animate monster HP bars
    bool graphics_updated = animate_monster_hp_bars();

    // Done when the battle text is finished an no more updates are performed.
    if (!graphics_updated && battle_text.state == BATTLE_TEXT_DONE) {
      animation_state = ANIMATION_COMPLETE;
    }
    break;
  }
}

void update_battle(void) NONBANKED {
  if (battle_state == BATTLE_INACTIVE)
    return;

  // Update animation timers, etc.
  if (update_timer(status_effect_timer)) {
    status_effect_frame ^= 1;
    reset_timer(status_effect_timer);
  }

  // Update the battle text
  battle_text.update();

  // TODO should this just always happen every frame?
  // redraw_player_status_effects();
  // redraw_monster_status_effects();

  switch (battle_state) {
  case BATTLE_STATE_MENU:
    update_battle_menu();
    break;
  case BATTLE_ROLL_INITIATIVE:
    roll_initiative();
    hide_cursor_sprites();
    battle_text.clear();
    show_battle_text();
    battle_state = BATTLE_NEXT_TURN;
    break;
  case BATTLE_NEXT_TURN:
    next_turn();
    if (encounter.round_complete) {
      battle_state = BATTLE_END_ROUND;
    } else {
      check_status_effects();
      battle_state = BATTLE_UPDATE_STATUS_EFFECTS;
    }
    break;
  case BATTLE_UPDATE_STATUS_EFFECTS:
    // TODO Handle this when implementing status effects
    battle_state =BATTLE_TAKE_ACTION;
    break;
  case BATTLE_TAKE_ACTION:
    take_action();
    battle_text.print(battle_pre_message);
    animation_state = ANIMATION_PREAMBLE;
    battle_state = BATTLE_ANIMATE;
    break;
  case BATTLE_ANIMATE:
    animate_action_result();
    if (animation_state == ANIMATION_COMPLETE) {
      battle_state = BATTLE_NEXT_TURN;
    }
    break;
  case BATTLE_PLAYER_FLED:
    // Flee success message / animation
    // Re
    break;
  case BATTLE_PLAYER_DIED:
    // Transition to the game over screen.
    break;
  case BATTLE_END_ROUND:
    // TODO Determine if the player can take actions
    battle_state = BATTLE_STATE_MENU;
    battle_menu = BATTLE_MENU_MAIN;
    hide_battle_text();
    move_cursor(BATTLE_CURSOR_MAIN_FIGHT);
    break;
  case BATTLE_SUCCESS:
    // Calculate and display rewards
    break;
  case BATTLE_LEVEL_UP:
    // Handle level up messages and logic.
    break;
  case BATTLE_COMPLETE:
    // Fade out and return to the world map.
    battle_state = BATTLE_INACTIVE;
    break;
  }
}

void draw_battle(void) NONBANKED {
  move_bkg(0, 0);
}
