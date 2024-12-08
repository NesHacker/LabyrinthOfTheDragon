#ifndef _BATTLE_H
#define _BATTLE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"

// -----------------------------------------------------------------------------
// Constants & Macros
// -----------------------------------------------------------------------------

/**
 * Sprite id for the cursor icon.
 */
#define SPRITE_CURSOR 0

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

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/**
 * Enumeration of all status effects (buffs & debuffs) in the game.
 */
typedef enum StatusEffect {
  // Debuffs (Effects 0-7)
  DEBUFF_BLIND,
  DEBUFF_SCARED,
  DEBUFF_PARALZYED,
  DEBUFF_POISONED,
  DEBUFF_CONFUSED,
  DEBUFF_ASLEEP,
  DEBUFF_ATTACK_DOWN,
  DEBUFF_AC_DOWN,
  // Buffs (Effects 8-15)
  BUFF_UNUSED_0,
  BUFF_UNUSED_1,
  BUFF_UNUSED_2,
  BUFF_UNUSED_3,
  BUFF_UNUSED_4,
  BUFF_UNUSED_5,
  BUFF_ATTACK_UP,
  BUFF_AC_UP,
} StatusEffect;

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
  BATTLE_MENU_MAGIC_TECH,
  BATTLE_MENU_ITEM,
  BATTLE_MENU_SUMMON,
} BattleMenu;

/**
 * Cursor position enumeration for various battle menus.
 */
typedef enum BattleCursor {
  BATTLE_CURSOR_MAIN_FIGHT,
  BATTLE_CURSOR_MAIN_MAGIC_TECH,
  BATTLE_CURSOR_MAIN_ITEM,
  BATTLE_CURSOR_MAIN_MAIN_SUMMON,
  BATTLE_CURSOR_ITEM_1,
  BATTLE_CURSOR_ITEM_2,
  BATTLE_CURSOR_ITEM_3,
  BATTLE_CURSOR_ITEM_4,
  BATTLE_CURSOR_ITEM_5,
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

/**
 * Data representation of a monster.
 */
typedef struct Monster {
  uint8_t id;
  char name[9];
  uint8_t tile_bank;
  uint8_t *tile_data;
  palette_color_t *palette;

  uint8_t max_hp;
  uint8_t max_mp;
  uint8_t attack;
  uint8_t ac;
  uint8_t magic_attack;
  uint8_t saving_throw;
  uint8_t initiative;

  // TODO AI Callbacks?
  // void (*attack)(...);
} Monster;

/**
 * Instanced monster that's used for battle.
 */
typedef struct MonsterInstance {
  uint8_t id;
  uint8_t hp;
  uint8_t mp;
  Monster *monster;
} MonsterInstance;

/**
 * Creates a new instance of the given monster.
 * @param id Id for the instance.
 * @param monster Pointer to the monster this is an instance of.
 * @return A new MonsterInstance for the monster.
 */
inline MonsterInstance monster_instance(uint8_t id, Monster *m) {
  MonsterInstance i = {
    id,
    m->max_hp,
    m->max_mp,
    m
  };
  return i;
}

// -----------------------------------------------------------------------------
// Externs & Prototypes
// -----------------------------------------------------------------------------

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
 * Moves the cursor sprite to the given column and row on the screen.
 * @param col Column for the cursor.
 * @param row Row for the cursor.
 */
inline void move_cursor_sprite(uint8_t col, uint8_t row) {
  const uint8_t x = (col + 1) * 8;
  const uint8_t y = (row + 2) * 8 - 1;
  move_sprite(SPRITE_CURSOR, x, y);
}

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

#endif
