/**
 * Main game state and functions.
 */
#ifndef _MAIN_H
#define _MAIN_H

/**
 * Enumeration of all main states for the game.
 */
typedef enum GameState {
  GAME_STATE_TITLE,
  GAME_STATE_WORLD_MAP,
  GAME_STATE_BATTLE,
  GAME_STATE_MAIN_MENU,
} GameState;

/**
 * Main state for the game. Determines subsystem controller run during the
 * core game and rendering loops.
 */
extern GameState game_state;

#endif
