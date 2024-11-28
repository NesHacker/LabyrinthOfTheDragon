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


// Function prototypes
void render(void);
void set_game_state(GameState s);

#endif
