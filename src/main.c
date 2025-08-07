#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdint.h>

#include "battle.h"
#include "credits.h"
#include "core.h"
#include "hero_select.h"
#include "map.h"
#include "sound.h"
#include "stats.h"
#include "test.h"
#include "title_screen.h"

GameState game_state = GAME_STATE_TITLE;
uint8_t joypad_down;
uint8_t joypad_pressed;
uint8_t joypad_released;

/**
 * Enumerates initial game modes. These are used in testing and development to
 * immediately jump to a specific test state.
 */
typedef enum InitialGameMode {
  GAME_MODE_NORMAL,
  GAME_MODE_HERO_SELECT,
  GAME_MODE_TEST_LEVEL,
  GAME_MODE_TEST_BATTLE,
  GAME_MODE_TEST_CREDITS,
} InitialGameMode;

/**
 * Determines the initial game mode for the game.
 */
const InitialGameMode initial_mode = GAME_MODE_NORMAL;

/**
 * Uncomment to enable sound effect testing when pressing the 'B' button.
 */
// #define SFX_TEST

/**
 * Random Seed to use for the game. If set to 0 then the game will generate a
 * seed value on the title while waiting for the player to begin the game.
 */
#define RANDOM_SEED 50

/**
 * Initializes the core game engine.
 */
static inline void initialize(void) {
  ENABLE_RAM;

  initarand(RANDOM_SEED);
  hide_window();

  switch (initial_mode) {
  case GAME_MODE_NORMAL:
    init_title_screen();
    game_state = GAME_STATE_TITLE;
    break;
  case GAME_MODE_HERO_SELECT:
    init_hero_select();
    game_state = GAME_STATE_HERO_SELECT;
    break;
  case GAME_MODE_TEST_LEVEL:
    test_level();
    break;
  case GAME_MODE_TEST_BATTLE:
    test_battle();
    break;
  case GAME_MODE_TEST_CREDITS:
    init_credits();
    break;
  }
}

/**
 * Executes core gameloop logic.
 */
static inline void game_loop(void) {
  switch (game_state) {
  case GAME_STATE_TITLE:
    update_title_screen();
    break;
  case GAME_STATE_HERO_SELECT:
    update_hero_select();
    break;
  case GAME_STATE_WORLD_MAP:
    update_world_map();
    break;
  case GAME_STATE_BATTLE:
    update_battle();
    break;
  case GAME_STATE_CREDITS:
    update_credits();
    break;
  }
}

/**
 * Executes rendering logic that must occur during a VBLANK.
 */
static inline void render(void) {
  switch (game_state) {
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
