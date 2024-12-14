#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdint.h>

#include "battle.h"
#include "data.h"
#include "joypad.h"
#include "main.h"
#include "map.h"
#include "main_menu.h"
#include "textbox.h"
#include "util.h"

uint8_t joypad_down;
uint8_t joypad_pressed;
uint8_t joypad_released;
GameState game_state = MAIN_MENU;

/**
 * Sets the main game state and executes any initialization code related to that
 * state.
 * @param s Game state to set.
 */
void set_game_state(GameState s) NONBANKED {
  switch (game_state = s) {
  case MAIN_MENU:
    init_main_menu();
    break;
  case WORLD_MAP:
    init_world_map();
    break;
  }
}

/**
 * Initializes the game.
 */
inline void initialize(void) {
  // init_main_menu();

  // init_text_box();
  // init_world_map();

  // Battle Testing
  initarand(100);
  init_player();
  init_battle();
}

/**
 * Executes core gameloop logic.
 */
inline void game_loop(void) {
  // update_main_menu();
  // update_world_map();
  update_battle();
}

/**
 * Executes rendering logic that must occur during a VBLANK.
 */
inline void render(void) {
  // draw_main_menu();
  // draw_world_map();
  draw_battle();
}

/**
 * Main function for the game. Handles initialization, game loop setup, and
 * joypad state updates.
 */
void main(void) {
  // Initialize the game state
  lcd_off();
  LCDC_REG = LCDCF_OFF
    | LCDCF_OBJON
    | LCDCF_BGON
    | LCDCF_WINON
    | LCDCF_WIN9C00;
  initialize();
  lcd_on();

  // Start the main game loop
  while (1) {
    // Update joypad bitfields
    uint8_t last = joypad_down;
    joypad_down = joypad();
    joypad_pressed = ~last & joypad_down;
    joypad_released = last & ~joypad_down;

    // Execute game logic
    game_loop();

    // Wait for a VBLANK then execute rendering logic
    vsync();
    render();
  }
}
