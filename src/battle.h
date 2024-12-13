#ifndef _BATTLE_H
#define _BATTLE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "flags.h"
#include "monster.h"
#include "player.h"
#include "tables.h"

// -----------------------------------------------------------------------------
// Constants & Macros
// -----------------------------------------------------------------------------

/**
 * First sprite id for the cursor icon.
 */
#define CURSOR_SPRITE 0

/**
 * Tile for the first cursor sprite.
 */
#define CURSOR_TILE 0x8A

/**
 * Sprite attribute for the cursor.
 */
#define CURSOR_ATTR 0x0F

/**
 * Clear tile to use when the battle controller is running.
 */
#define BATTLE_CLEAR_TILE 0xFF

/**
 * Clear attribute to use when the battle controller is running.
 */
#define BATTLE_CLEAR_ATTR 0x08

/**
 * Attribute to use for buff icon tiles.
 */
#define BUFF_ATTRIBUTE 0x0E

/**
 * Attribute to use for debuff icon tiles.
 */
#define DEBUFF_ATTRIBUTE 0x0F

/**
 * Tile id for the fight icon in the font tileset.
 */
#define FIGHT_ICON 0x96

/**
 * Tile id for the magic icon in the font tileset.
 */
#define MAGIC_ICON 0x9A

/**
 * Tile id for the tech icon in the font tileset.
 */
#define TECH_ICON 0xFB

/**
 * Tile id for the item icon in the font tileset.
 */
#define ITEM_ICON 0xA8

/**
 * Tile id for the summon icon in the font tileset.
 */
#define SUMMON_ICON 0xDB

/**
 * Tile id for the flee icon in the font tileset.
 */
#define FLEE_ICON 0xBB

/**
 * Tile id for the left tile of the "MP" label.
 */
#define MP_ICON_LEFT 0x8E

/**
 * Tile id for the right tile of the "MP" label.
 */
#define MP_ICON_RIGHT 0x8F

/**
 * Tile id for the left tile of the "SP" label.
 */
#define SP_ICON_LEFT 0x9E

/**
 * Tile id for the right tile of the "SP" label.
 */
#define SP_ICON_RIGHT 0x9F

/**
 * ROM bank in which most data and routines related to battle are stored. This
 * bank must be loaded in order for the battle system to properly function.
 * @see load_battle_bank()
 */
#define BATTLE_ROM_BANK 3

/**
 * Denotes the starting row in the background where the menu graphics begin.
 */
#define MENU_Y 11

/**
 * Denotes the starting row in the background where the submenu graphics begin.
 */
#define SUBMENU_Y 18

/**
 * Row where submenu body text begins.
 */
#define SUBMENU_TEXT_Y 19

/**
 * Column where submenu body text begins.
 */
#define SUBMENU_TEXT_X 5

/**
 * VRAM address for the main menu summon text in the background.
 */
#define VRAM_SUMMON_NAME VRAM_BACKGROUND_XY(10, MENU_Y + 1)

/**
 * VRAM address for the starting tile of submenu text in the background.
 */
#define VRAM_SUBMENU_TEXT VRAM_BACKGROUND_XY(SUBMENU_TEXT_X, SUBMENU_TEXT_Y)

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/**
 * Main state enumeration for the battle system.
 */
typedef enum BattleState {
  BATTLE_STATE_MENU,
  // BATTLE_STATE_TEXT,
} BattleState;

/**
 * Enumeration battle menus that can be displayed at the bottom of the screen.
 */
typedef enum BattleMenu {
  BATTLE_MENU_MAIN,
  BATTLE_MENU_FIGHT,
  BATTLE_MENU_ABILITY,
  BATTLE_MENU_ITEM,
  BATTLE_MENU_SUMMON,
} BattleMenu;

/**
 * Cursor position enumeration for various battle menus.
 */
typedef enum BattleCursor {
  BATTLE_CURSOR_MAIN_FIGHT,
  BATTLE_CURSOR_MAIN_ABILITY,
  BATTLE_CURSOR_MAIN_ITEM,
  BATTLE_CURSOR_MAIN_MAIN_SUMMON,
  BATTLE_CURSOR_MAIN_FLEE,
  BATTLE_CURSOR_ITEM_1,
  BATTLE_CURSOR_ITEM_2,
  BATTLE_CURSOR_ITEM_3,
  BATTLE_CURSOR_ITEM_4,
  BATTLE_CURSOR_ITEM_5,
  BATTLE_CURSOR_NO_ITEMS,
  BATTLE_CURSOR_MONSTER_1,
  BATTLE_CURSOR_MONSTER_2,
  BATTLE_CURSOR_MONSTER_3,
} BattleCursor;

/**
 * Enumerates the four possible monster layouts for a battle.
 */
typedef enum MonsterLayout {
  /**
   * One monster of any type (Sm, Md, Lg).
   */
  MONSTER_LAYOUT_1,
  /**
   * Two monsters of any type (Sm, Md, Lg).
   */
  MONSTER_LAYOUT_2,
  /**
   * Three small monsters.
   */
  MONSTER_LAYOUT_3S,
  /**
   * One medium and two small monsters.
   */
  MONSTER_LAYOUT_1M_2S,
} MonsterLayout;

/**
 * Enumeration of the three major layouts for the battle menu at the bottom of
 * the screen.
 */
typedef enum BattleMenuLayout {
  /**
   * Main menu w/ player stats.
   */
  BATTLE_MENU_LAYOUT_MAIN,
  /**
   * Submenus for "Fight", "Magic", "Tech", and "Summon".
   */
  BATTLE_MENU_LAYOUT_SUBMENU,
  /**
   * Text menu for describing the actions and results of rolls.
   */
  BATTLE_MENU_LAYOUT_TEXT,
} BattleMenuLayout;

/**
 * Enumeration of monster positions on the battle screen.
 */
typedef enum MonsterPosition {
  MONSTER_POSITION1,
  MONSTER_POSITION2,
  MONSTER_POSITION3,
} MonsterPosition;

// -----------------------------------------------------------------------------
// Externs & Prototypes
// -----------------------------------------------------------------------------

/**
 * Default palettes for the battle system.
 */
extern const palette_color_t battle_bg_palettes[];

/**
 * Default sprite palettes for the battle system.
 */
extern const palette_color_t battle_sprite_palettes[];


/**
 * Main state variable for the battle system.
 */
extern BattleState battle_state;

/**
 * Which menu is currently open on the battle system.
 */
extern BattleMenu battle_menu;

/**
 * Cursor state for battle system menus.
 */
extern BattleCursor battle_cursor;

/**
 * The monster layout for the current battle.
 */
extern MonsterLayout battle_monster_layout;

/**
 * The instanced monsters for the current battle.
 */
extern MonsterInstance battle_monsters[];

/**
 * Number of selectable submenu items. General limit value for the abilities,
 * items, and summons submenus.
 */
extern uint8_t battle_num_submenu_items;

/**
 * Initializes the battle system.
 */
void init_battle(void);

/**
 * Executed when battle is complete and before transitioning back to map mode.
 */
void cleanup_battle(void);

/**
 * Game loop update for the battle system.
 */
void update_battle(void);

/**
 * VBLANK draw updates for the battle system.
 */
void draw_battle(void);

// -----------------------------------------------------------------------------
// Inline functions
// -----------------------------------------------------------------------------

/**
 * Determines the x position for the HP bar based on the given monster position
 * and the current monster layout.
 * @param pos Position of the monster on the screen.
 * @return The x position for the HP bar.
 */
inline uint8_t get_hp_bar_x(MonsterPosition pos) {
  switch (battle_monster_layout) {
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
  switch (battle_monster_layout) {
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
 * @return A pointer to monster instance at the given monster position in the
 *   current battle.
 * @param pos Position for the monster.
 */
inline MonsterInstance *get_monster_at(MonsterPosition pos) {
  return battle_monsters + sizeof(MonsterInstance) * pos;
}

/**
 * @return `true` If the submenu contains a number of selectable items.
 */
inline bool has_submenu_items(void) {
  return battle_num_submenu_items > 0;
}

#endif
