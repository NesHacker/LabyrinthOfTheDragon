#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdint.h>

#include "battle.h"
#include "core.h"
#include "main.h"
#include "map.h"
#include "main_menu.h"

uint8_t joypad_down;
uint8_t joypad_pressed;
uint8_t joypad_released;
GameState game_state = GAME_STATE_TITLE;

/**
 * Initializes the core game engine.
 */
inline void initialize(void) {
  initarand(RANDOM_SEED);
  init_player();
  move_win(7, 144);

  // init_main_menu();
  // init_world_map();
  init_battle();

  // game_state = GAME_STATE_WORLD_MAP;
  game_state = GAME_STATE_BATTLE;
}

/**
 * Executes core gameloop logic.
 */
inline void game_loop(void) {
  switch (game_state) {
  case GAME_STATE_TITLE:
    update_main_menu();
    break;
  case GAME_STATE_WORLD_MAP:
    update_world_map();
    break;
  case GAME_STATE_BATTLE:
    update_battle();
    break;
  }
}

/**
 * Executes rendering logic that must occur during a VBLANK.
 */
inline void render(void) {
  switch (game_state) {
  case GAME_STATE_TITLE:
    draw_main_menu();
    break;
  case GAME_STATE_WORLD_MAP:
    draw_world_map();
    break;
  case GAME_STATE_BATTLE:
    draw_battle();
    break;
  }
}

/**
 * Reads and updates the joypad state.
 */
inline void update_joypad(void) {
  uint8_t last = joypad_down;
  joypad_down = joypad();
  joypad_pressed = ~last & joypad_down;
  joypad_released = last & ~joypad_down;
}

/**
 * Main function for the game. Handles initialization, game loop setup, and
 * joypad state updates.
 */
void main(void) {
  lcd_off();
  LCDC_REG = LCDCF_OFF
    | LCDCF_OBJON
    | LCDCF_BGON
    | LCDCF_WINON
    | LCDCF_WIN9C00;
  initialize();
  lcd_on();

  while (1) {
    update_joypad();
    text_writer.update();
    game_loop();
    vsync();
    render();
  }
}
