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

BattleState battle_state;
BattleMenu battle_menu;
BattleCursor battle_cursor;
MonsterLayout battle_monster_layout;
uint8_t battle_num_submenu_items;

uint8_t battle_num_monsters;
MonsterInstance battle_monsters[] = {{0}, {0}, {0}};

Timer status_effect_timer;
uint8_t status_effect_frame = 0;

/**
 * Sets and draws the monster layout.
 * @param layout Monster layout to draw to the background.
 */
void set_monster_layout(MonsterLayout layout) {
  battle_monster_layout = layout;
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

  // TODO: Instancing of the monsters should be handled by the encounter
  //       system. This method should simply load an instance into a slot and
  //       copy the monster's tiles to VRAM.
  MonsterInstance *instance = get_monster_at(p);
  monster->new_instance(p, 1, C_TIER);

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
    update_bg_palettes(1, 1, instance->palette);
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
    update_bg_palettes(2, 1, instance->palette);
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
    update_bg_palettes(3, 1, instance->palette);
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
  for (uint8_t k = 0; k < 7; k++, vram++) {
    VBK_REG = VBK_TILES;
    set_vram_byte(vram, BATTLE_CLEAR_TILE);
    VBK_REG = VBK_ATTRIBUTES;
    set_vram_byte(vram, BATTLE_CLEAR_ATTR);
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
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);

  // Calculate the number of pips based on the ratio of HP to Max HP
  uint16_t k = (40 * hp) / max;
  uint8_t p = (uint8_t)k;

  // If p <= 13, then HP <= (1/3) * max, so set critical palette
  uint8_t attr = (p <= 13) ? 0b00001101 : 0b00001100;
  uint8_t col = 0;

  // Draw the "left cap"
  VBK_REG = VBK_TILES;
  set_vram_byte(vram, 0x50);
  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram++, attr);

  // Fill full bar sprites until the pips value falls below 8
  while (p >= 8) {
    VBK_REG = VBK_TILES;
    set_vram_byte(vram, 0x51);
    VBK_REG = VBK_ATTRIBUTES;
    set_vram_byte(vram++, attr);
    p -= 8;
    col++;
  }

  // If we've not filled all the pip sprites, then draw the next based on the
  // remaining pips
  if (col < 5) {
    VBK_REG = VBK_TILES;
    set_vram_byte(vram, 0x52 + p);
    VBK_REG = VBK_ATTRIBUTES;
    set_vram_byte(vram++, attr);
    col++;
  }

  // Draw any remaining pip sprites as "empty"
  while (col < 5) {
    VBK_REG = VBK_TILES;
    set_vram_byte(vram, 0x52);
    VBK_REG = VBK_ATTRIBUTES;
    set_vram_byte(vram++, attr);
    col++;
  }

  // Finally draw the "right cap"
  VBK_REG = VBK_TILES;
  set_vram_byte(vram, 0x5A);
  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram++, attr);
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
    move_cursor_sprites(3, 12);
    break;
  case BATTLE_CURSOR_ITEM_2:
    move_cursor_sprites(3, 13);
    break;
  case BATTLE_CURSOR_ITEM_3:
    move_cursor_sprites(3, 14);
    break;
  case BATTLE_CURSOR_ITEM_4:
    move_cursor_sprites(3, 15);
    break;
  case BATTLE_CURSOR_ITEM_5:
    move_cursor_sprites(3, 16);
    break;
  case BATTLE_CURSOR_NO_ITEMS:
    hide_cursor_sprites();
  case BATTLE_CURSOR_MONSTER_1:
    switch (battle_monster_layout) {
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
    switch (battle_monster_layout) {
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
    switch (battle_monster_layout) {
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

void init_battle(void) {
  lcd_off();

  // Switch to the battle ROM bank (has stats tables, etc.)
  SWITCH_ROM(BATTLE_ROM_BANK);

  // TODO Load player & skills, and monsters
  init_player(); // TODO remove me after testing
  player.summon->activate();

  // Reset the background and window position
  fill_background(BATTLE_CLEAR_TILE, BATTLE_CLEAR_ATTR);
  move_win(0, 144);

  // Load Palettes for the battle system
  update_bg_palettes(0, 8, battle_bg_palettes);
  update_sprite_palettes(0, 8, battle_sprite_palettes);

  // Draw the monster layout and instantiate the monsters for the fight
  // TODO Load this data based on the fight type
  set_monster_layout(MONSTER_LAYOUT_2);
  load_monster(MONSTER_POSITION1, &MONSTER_KOBOLD);
  load_monster(MONSTER_POSITION2, &MONSTER_KOBOLD);

  // Load the font and battle tilesets
  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_font, VRAM_SHARED_TILES);
  load_tiles(1, tile_battle, (void *)(0x9300), 5 * 16);

  // Draw menus and initialize cursor
  draw_battle_menu(BATTLE_MENU_LAYOUT_MAIN, VRAM_BACKGROUND_XY(0, MENU_Y));
  draw_battle_menu(BATTLE_MENU_LAYOUT_SUBMENU, VRAM_BACKGROUND_XY(0, SUBMENU_Y));
  draw_battle_menu(BATTLE_MENU_LAYOUT_TEXT, VRAM_WINDOW_XY(0, 0));

  draw_text(VRAM_SUMMON_NAME, player.summon->name, 9);
  set_magic_or_tech_menu();

  print_fraction(VRAM_BACKGROUND_XY(12, 14), player.hp, player.max_hp);
  print_fraction(VRAM_BACKGROUND_XY(12, 15), player.sp, player.max_sp);

  init_cursor();

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
  // CRITICAL {
  //   remove_LCD(fight_menu_isr);
  //   remove_LCD(nowait_int_handler);
  //   STAT_REG = 0;
  // }
  // set_interrupts(IE_REG & ~LCD_IFLAG);
}

/**
 * Confirms that the player has chosen the default "fight" action and begins the
 * next round of combat.
 */
void confirm_fight(void) {
}

/**
 * Confirms that the player has chosen an ability action and begins the next
 * round of combat.
 */
void confirm_ability(void) {
}

/**
 * Confirms that the player has chosen to use an item and begins the next round
 * of combat.
 */
void confirm_item(void) {
}

/**
 * Confirms the player has chosen a summon and begins the next round of combat.
 */
void confirm_summon(void) {
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
  switch (battle_monster_layout) {
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
  switch (battle_monster_layout) {
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
  draw_text(VRAM_BACKGROUND_XY(8, 21), "EMPTY\x60", 6);
}

/**
 * Clears the and resets the battle submenu.
 */
void reset_battle_submenu(void) {
  // Clear menu state & hide the cursor
  battle_num_submenu_items = 0;
  hide_cursor_sprites();

  // Set the submenu icon
  switch (battle_menu) {
  case BATTLE_MENU_ABILITY:
    set_tile_xy(1, 19, is_magic_class() ? MAGIC_ICON : TECH_ICON);
    break;
  case BATTLE_MENU_ITEM:
    set_tile_xy(1, 19, ITEM_ICON);
    break;
  case BATTLE_MENU_SUMMON:
    set_tile_xy(1, 19, SUMMON_ICON);
    break;
  }

  // Clear the body of the menu
  uint8_t *vram = VRAM_BACKGROUND_XY(3, 19);
  for (uint8_t y = 19; y < 24; y++) {
    for (uint8_t x = 3; x < 19; x++)
      set_vram_byte(vram++, FONT_SPACE);
    vram += 16;
  }
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
    draw_text(vram, ability->name, 10);
    uint8_t *cost_tile = ability->sp_cost_tiles;
    set_vram_byte(vram + 11, *cost_tile++);
    set_vram_byte(vram + 12, *cost_tile++);
    set_vram_byte(vram + 13, *cost_tile++);
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

/**
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

void update_battle(void) {
  if (update_timer(status_effect_timer)) {
    status_effect_frame ^= 1;
    reset_timer(status_effect_timer);
  }
  redraw_player_status_effects();
  redraw_monster_status_effects();

  switch (battle_state) {
  case BATTLE_STATE_MENU:
    update_battle_menu();
    break;
  }
}

void draw_battle(void) {
  move_bkg(0, 0);
}
