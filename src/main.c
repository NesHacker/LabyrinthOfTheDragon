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
Timer sound_update_timer;

/**
 * Timer interrupt handler. Primarily responsible for performing sound engine
 * updates at a fixed rate.
 */
void timer_isr(void) {
  if (update_timer(sound_update_timer)) {
    update_sound();
    reset_timer(sound_update_timer);
  }
}

/**
 * Initializes the normal game. Abstracted out of `initialize` to make it easy
 * to switch between tests and the actual game while handling common setup.
 */
void initialize_game(void) {
  init_player(CLASS_DRUID);
  init_world_map();
  game_state = GAME_STATE_WORLD_MAP;
}

/**
 * Initializes the core game engine.
 */
inline void initialize(void) {
  ENABLE_RAM;

  initarand(RANDOM_SEED);
  hide_window();

  // initialize_game();
  // test_combat_general(MONSTER_LAYOUT_1, DUMMY_NORMAL);
  // test_flee();
  test_big_map();
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
  case GAME_STATE_TEST:
    return;
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
  case GAME_STATE_TEST:
    return;
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
 * Initializesd the sound engine and sets the timer interrupt.
 */
void init_sound_engine(void) {
  sound_init();
  init_timer(sound_update_timer, 4);
  add_TIM(timer_isr);
  TAC_REG = 0b00000110;
  set_interrupts(TIM_IFLAG | VBL_IFLAG);
}

/**
 * Main function for the game. Handles initialization, game loop setup, and
 * joypad state updates.
 */
void main(void) {
  // TODO Send to "YOU NEED GBC screen if not"
  if (_cpu == CGB_TYPE)
    cpu_fast();

  disable_interrupts();
  DISPLAY_OFF;

  init_sound_engine();
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

    if (was_pressed(J_B))
      sfx_test();

    game_loop();
    vsync();
    render();
  }
}
