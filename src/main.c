#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

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
  init_text_box();
  // init_main_menu();
  init_world_map();
  game_state = WORLD_MAP;
}

/**
 * Executes core gameloop logic.
 */
inline void game_loop(void) {
  switch (game_state) {
  case MAIN_MENU:
    update_main_menu();
    break;
  case WORLD_MAP:
    update_world_map();
    break;
  }
}

/**
 * Executes rendering logic that must occur during a VBLANK.
 */
inline void render(void) {
  switch (game_state) {
  case MAIN_MENU:
    draw_main_menu();
    break;
  case WORLD_MAP:
    draw_world_map();
    break;
  }
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
