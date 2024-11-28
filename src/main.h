/**
 * Main game state and functions.
 */
#ifndef _MAIN_H
#define _MAIN_H

/**
 * Enumeration of all main states for the game.
 */
typedef enum GameState {
  MAIN_MENU
} GameState;

/**
 * Initializes graphics, sound, and game state.
 */
void init_game(void);

/**
 * Executes core gameloop logic.
 */
void game_loop(void);

/**
 * Executes rendering logic that must occur during a VBLANK.
 */
void render(void);

/**
 * Sets the main game state.
 * @param s Game state to set.
 */
void set_game_state(GameState s);

#endif
