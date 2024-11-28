#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "joypad.h"
#include "main.h"
#include "main_menu.h"
#include "textbox.h"
#include "util.h"

uint8_t joypad_down;
uint8_t joypad_pressed;
uint8_t joypad_released;

GameState game_state = MAIN_MENU;

void set_game_state(GameState s) NONBANKED {
  game_state = s;

  switch (game_state) {
  case MAIN_MENU:
    lcd_off();
    init_main_menu();
    lcd_on();
    break;
  }
}

void init_game(void) {
  load_tilesets();
  init_text_box();
  init_main_menu();
}

void game_loop(void) {
  switch (game_state) {
  case MAIN_MENU:
    update_main_menu();
    break;
  }
}

void render(void) {
  switch (game_state) {
  case MAIN_MENU:
    draw_main_menu();
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
  init_game();
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
