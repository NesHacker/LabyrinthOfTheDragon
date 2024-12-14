#include <gb/gb.h>
#include <stdint.h>
#include <stdlib.h>

#include "battle_text.h"
#include "player.h"

/**
 * Maps player `message_delay` value to a number of frames to wait between pages
 * or messages.
 */
const uint8_t message_speed_to_frames[] = { 60, 45, 30, 20, 15 };

/**
 * @return The number of frames to delay between pages based on the player's
 *   configured message speed.
 */
uint8_t page_delay_frames(void) {
  return message_speed_to_frames[player.message_speed];
}

/**
 * Helper function that transitions to a character delay.
 */
inline void set_char_delay(void) {
  init_timer(battle_text.timer, BATTLE_TEXT_CHAR_DELAY_FRAMES);
  battle_text.state = BATTLE_TEXT_CHAR_DELAY;
}

/**
 * Helper function that transitions to a page delay.
 */
inline void set_page_delay(void) {
  init_timer(battle_text.timer, page_delay_frames());
  battle_text.state = BATTLE_TEXT_PAGE_DELAY;
}

/**
 * Helper function that transitions to the end text delay.
 */
inline void set_end_delay(void) {
  init_timer(battle_text.timer, page_delay_frames());
  battle_text.state = BATTLE_TEXT_END_DELAY;
}

/**
 * Clears the battle text box.
 */
void battle_text_clear(void) {
  uint8_t *vram = VRAM_BATTLE_TEXT;
  for (uint8_t y = 0; y < BATTLE_TEXT_HEIGHT; y++) {
    for (uint8_t x = 0; x < BATTLE_TEXT_WIDTH; x++)
      set_vram_byte(vram++, FONT_SPACE);
    vram += VRAM_ROW_OFFSET(19) + 1;
  }
}

/**
 * Begins printing the given string to the battle textbox.
 * @param string Pre-encoded string to print.
 */
void battle_text_print(const char *string) {
  battle_text.text = string;
  battle_text.state = BATTLE_TEXT_START;
}

/**
 * Game loop update handler for the battle textbox.
 */
void battle_text_update(void) {
  switch (battle_text.state) {
  case BATTLE_TEXT_START:
  case BATTLE_TEXT_CLEAR:
    battle_text_clear();
    battle_text.col = 0;
    battle_text.line = 0;
    battle_text.vram = VRAM_BATTLE_TEXT + 0x20;
    battle_text.state = BATTLE_TEXT_WRITING;
    return;
  case BATTLE_TEXT_WRITING:
    char next = *battle_text.text++;
    switch (next) {
    case '\0':
      set_end_delay();
      return;
    case '\n':
      if (battle_text.line == 3)
        set_page_delay();
      else
        battle_text.vram += VRAM_ROW_OFFSET(battle_text.col + 1) + 1;
      return;
    case '\f':
      set_page_delay();
      return;
    default:
      // Note: `strings2c` should preformat the text so that the maximum column
      //       length will never be exceeded, so no need to handle this case.
      battle_text.col++;
      VBK_REG = VBK_TILES;
      set_vram_byte(battle_text.vram++, next + FONT_OFFSET);
      set_char_delay();
    }
    return;
  case BATTLE_TEXT_CHAR_DELAY:
    if (update_timer(battle_text.timer)) {
      battle_text.state = BATTLE_TEXT_WRITING;
    }
    return;
  case BATTLE_TEXT_PAGE_DELAY:
    if (update_timer(battle_text.timer)) {
      battle_text.state = BATTLE_TEXT_CLEAR;
    }
    return;
  case BATTLE_TEXT_END_DELAY:
    if (update_timer(battle_text.timer)) {
      battle_text.state = BATTLE_TEXT_DONE;
    }
    return;
  case BATTLE_TEXT_DONE:
  default:
  }
}

BattleText battle_text = {
  battle_text_clear,
  battle_text_print,
  battle_text_update,
  BATTLE_TEXT_DONE,
};
