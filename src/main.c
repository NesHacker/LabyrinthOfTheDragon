#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdint.h>

#include "battle.h"
#include "core.h"
#include "map.h"
#include "main_menu.h"
#include "sound.h"
#include "stats.h"
#include "test.h"

GameState game_state = GAME_STATE_TITLE;
uint8_t joypad_down;
uint8_t joypad_pressed;
uint8_t joypad_released;

/**
 * Uncomment to enable sound effect testing when pressing the 'B' button.
 */
// #define SFX_TEST

/**
 * Random Seed to use for the game. If set to 0 then the game will generate a
 * seed value on the title while waiting for the player to begin the game.
 */
#define RANDOM_SEED 50

#define SFX_TEST

/**
 * Initializes the normal game. Abstracted out of `initialize` to make it easy
 * to switch between tests and the actual game while handling common setup.
 */
static void initialize_game(void) {
  init_player(CLASS_DRUID);
  init_world_map();
  game_state = GAME_STATE_WORLD_MAP;
}

/**
 * Initializes the core game engine.
 */
static inline void initialize(void) {
  ENABLE_RAM;

  initarand(RANDOM_SEED);
  hide_window();

  test_level();
  // test_battle();
}

/**
 * Executes core gameloop logic.
 */
static inline void game_loop(void) {
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
  case GAME_STATE_TEST:
    return;
  }
}

/**
 * Executes rendering logic that must occur during a VBLANK.
 */
static inline void render(void) {
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
  case GAME_STATE_TEST:
    return;
  }
}

/**
 * Reads and updates the joypad state.
 */
static inline void update_joypad(void) {
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
  if (_cpu == CGB_TYPE)
    cpu_fast();
  else {
    while(1) {}
  }

  disable_interrupts();
  DISPLAY_OFF;

  sound_init();

  LCDC_REG = LCDCF_OFF
    | LCDCF_OBJON
    | LCDCF_BGON
    | LCDCF_WINON
    | LCDCF_WIN9C00;
  initialize();

  DISPLAY_ON;

  enable_interrupts();

  while (1) {
    update_joypad();

    #ifdef SFX_TEST
    if (was_pressed(J_B))
      play_sound(sfx_test);
    #endif

    game_loop();
    vsync();
    render();
  }
}
