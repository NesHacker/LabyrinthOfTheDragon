#ifndef _BATTLE_TEXT_H
#define _BATTLE_TEXT_H

#include <stdint.h>

#include "data.h"
#include "util.h"

/**
 * VRAM location for the top left character of the battle text box.
 */
#define VRAM_BATTLE_TEXT VRAM_WINDOW_XY(1, 2)

/**
 * Height of the battle textbox.
 */
#define BATTLE_TEXT_HEIGHT 4

/**
 * Width of the battle textbox.
 */
#define BATTLE_TEXT_WIDTH 18

/**
 * Delay in frames between each character.
 */
#define BATTLE_TEXT_CHAR_DELAY_FRAMES 1

/**
 * States for the battle message controller.
 */
typedef enum BattleTextState {
  /**
   * Text has been initialized.
   */
  BATTLE_TEXT_START,
  /**
   * The text box is being cleared prior to writing further characters.
   */
  BATTLE_TEXT_CLEAR,
  /**
   * The text box is writing the next character to the screen.
   */
  BATTLE_TEXT_WRITING,
  /**
   * The text box is delaying before writing the next character.
   */
  BATTLE_TEXT_CHAR_DELAY,
  /**
   * The text box is delaying before clearing and beginning a new page of text.
   * This delay is determined by the players `message_speed` member.
   */
  BATTLE_TEXT_PAGE_DELAY,
  /**
   * The text box has finished writing text and is delaying before completing.
   * This delay is determined by the players `message_speed` member.
   */
  BATTLE_TEXT_END_DELAY,
  /**
   * Text has finished rendering and all waits have completed. The text as
   * shown will remain and the system does nothing upon update.
   */
  BATTLE_TEXT_DONE,
} BattleTextState;

/**
 * Battle text controller. Responsible for animating and drawing text onto the
 * screen as messages occur during battle. Uses the player model's message speed
 * to change how quickly message pages change.
 */
typedef struct BattleText {
  /**
   * Clears the textbox.
   */
  void (*clear)(void);
  /**
   * Initiates an animated print of the given string.
   * @param string Pre-encoded string to print to the battle text box.
   */
  void (*print)(const char *string);
  /**
   * Update method to be called during the game loop.
   */
  void (*update)(void);
  /**
   * Current state for the battle text system.
   */
  BattleTextState state;
  /**
   * Text currently being printed to the textbox.
   */
  const char *text;
  /**
   * Position in VRAM where the next character will be written.
   */
  uint8_t *vram;
  /**
   * Current column where text is being written.
   */
  uint8_t col;
  /**
   * Current row where text is being written.
   */
  uint8_t line;
  /**
   * Timer to use when handling delays.
   */
  Timer timer;
} BattleText;

/**
 * Battle text controller.
 */
extern BattleText battle_text;

#endif
