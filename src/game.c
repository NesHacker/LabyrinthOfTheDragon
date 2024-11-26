#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "game.h"
#include "graphics.h"
#include "joypad.h"
#include "main_menu.h"
#include "textbox.h"
#include "util.h"

GameState game_state = MAIN_MENU;

void set_game_state(GameState s) {
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
  load_palettes();
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
}
