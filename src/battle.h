#ifndef _BATTLE_H
#define _BATTLE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

/**
 * Sprite id for the cursor icon.
 */
#define SPRITE_CURSOR 0

/**
 * Main state enumeration for the battle system.
 */
typedef enum BattleState {
  BATTLE_STATE_MAIN_MENU,
  BATTLE_STATE_FIGHT,
  BATTLE_STATE_SKILL,
  BATTLE_STATE_TEXT,
} BattleState;

/**
 * Cursor position enumeration for various battle menus.
 */
typedef enum BattleCursor {
  BATTLE_CURSOR_FIGHT,
  BATTLE_CURSOR_MONSTER_MANUAL,
  BATTLE_CURSOR_SKILL,
  BATTLE_CURSOR_RUN,
  BATTLE_CURSOR_MOVE1,
  BATTLE_CURSOR_MOVE2,
  BATTLE_CURSOR_MOVE3,
  BATTLE_CURSOR_MOVE4,
} BattleCursor;

/**
 * Battle menu enumeration. Used when loading and rendering battle menus during
 * initialization.
 */
typedef enum BattleMenu {
  BATTLE_MENU_MAIN,
  BATTLE_MENU_TEXT,
  BATTLE_MENU_FIGHT,
} BattleMenu;

/**
 * Represents a player character skill that can be used in battle.
 */
typedef struct PlayerSkill {
  uint8_t id;
  char name[13];
  char type[8];
  uint8_t action_points;
  uint8_t max_action_points;
} PlayerSkill;

/**
 * Main state variable for the battle system.
 */
extern BattleState battle_state;

/**
 * Cursor state for battle system menus.
 */
extern BattleCursor battle_cursor;

/**
 * List of player skills available during battle.
 */
extern PlayerSkill battle_player_skills[4];

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

#endif
