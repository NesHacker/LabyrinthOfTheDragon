#ifndef _TEXT_WRITER_H
#define _TEXT_WRITER_H

#include "core.h"

/**
 * Number of frames to wait with a slow auto page speed.
 */
#define AUTO_PAGE_SLOW_FRAMES 60

/**
 * Number of frames to wait with a medium auto page speed.
 */
#define AUTO_PAGE_MED_FRAMES 30

/**
 * Number of frames to wait with a fast auto page speed.
 */
#define AUTO_PAGE_FAST_FRAMES 15

/**
 * Default number of frames to wait between characters.
 */
#define TEXT_WRITER_DEFAULT_CHARACTER_DELAY 2

/**
 * Default width for the text writing area.
 */
#define TEXT_WRITER_DEFAULT_WIDTH 18

/**
 * Default height for the text writing area.
 */
#define TEXT_WRITER_DEFAULT_HEIGHT 4

/**
 * Default column for the origin.
 */
#define TEXT_WRITER_DEFAULT_ORIGIN_COL 1

/**
 * Default row for the origin.
 */
#define TEXT_WRITER_DEFAULT_ORIGIN_ROW 1

/**
 * Default vram origin position for the text writer.
 */
#define TEXT_WRITER_DEFAULT_ORIGIN VRAM_WINDOW_XY(1, 1)

/**
 * States for the battle message controller.
 */
typedef enum TextWriterState {
  /**
   * Text has finished rendering and all waits have completed. The text as
   * shown will remain and the system does nothing upon update.
   */
  TEXT_WRITER_DONE,
  /**
   * The writer is waiting at the end of the page and will not continue until
   * the `next_page` method is called. This can only be reached if the auto
   * page speed is set to `AUTO_PAGE_OFF`.
   */
  TEXT_WRITER_PAGE_WAIT,
  /**
   * Text has been initialized.
   */
  TEXT_WRITER_START,
  /**
   * The text box is being cleared prior to writing further characters.
   */
  TEXT_WRITER_CLEAR,
  /**
   * The text box is writing the next character to the screen.
   */
  TEXT_WRITER_WRITING,
  /**
   * The text box is delaying before writing the next character.
   */
  TEXT_WRITER_CHAR_DELAY,
  /**
   * The text box is delaying before clearing and beginning a new page of text.
   * This state cannot be reached if the auto page speed is `AUTO_PAGE_OFF`.
   */
  TEXT_WRITER_PAGE_DELAY,
  /**
   * The text box has finished writing text and is delaying before completing.
   * This state can only be reached if the auto page speed is `AUTO_PAGE_OFF`.
   */
  TEXT_WRITER_END_DELAY,
} TextWriterState;

/**
 * Auto page speed settings for the text writer.
 */
typedef enum AutoPageSpeed {
  /**
   * Turns the auto-page feature off, requiring the controller to advance pages
   * via the `next_page` method.
   */
  AUTO_PAGE_OFF,
  /**
   * Sets the auto page speed to slow.
   */
  AUTO_PAGE_SLOW,
  /**
   * Sets the auto page speed to medium.
   */
  AUTO_PAGE_MED,
  /**
   * Sets the auto page speed to fast.
   */
  AUTO_PAGE_FAST,
} AutoPageSpeed;

/**
 * Text writer controller. Animates the writing of text to the screen.
 */
typedef struct TextWriter {
  /**
   * Sets the auto page speed.
   * @param s Auto page speed to set.
   */
  void (*set_auto_page)(AutoPageSpeed s);
  /**
   * Sets the render area size.
   * @param w Width in characters.
   * @param h Height in characters.
   */
  void (*set_size)(uint8_t w, uint8_t h);
  /**
   * Sets the text origin position in vram.
   * @param vram Root VRAM location.
   * @param col Tile column offset.
   * @param row Tile row offset.
   */
  void (*set_origin)(uint8_t *vram, uint8_t col, uint8_t row);
  /**
   * Clears the text area using `FONT_SPACE`.
   */
  void (*clear)(void);
  /**
   * Begins printing the given string.
   */
  void (*print)(const char *string);
  /**
   * Called to proceeed to the next page. Calling this outside of a page wait
   * will clear the screen and continue printing characters.
   */
  void (*next_page)(void);
  /**
   * Game loop update method. This method is called by the main game loop and
   * should not be called by system controllers.
   */
  void (*update)(void);
  /**
   * Current state for the text writer.
   */
  TextWriterState state;
  /**
   * Character width for the rendering area.
   */
  uint8_t width;
  /**
   * Character height for the rendering area.
   */
  uint8_t height;
  /**
   * Controls the page speed if auto paging is enabled.
   */
  AutoPageSpeed auto_page;
  /**
   * Number of frames to delay between characters.
   */
  uint8_t character_delay_frames;
  /**
   * Current text column.
   */
  uint8_t col;
  /**
   * Current text row.
   */
  uint8_t row;
  /**
   * Origin column.
   */
  uint8_t origin_column;
  /**
   * Origin row.
   */
  uint8_t origin_row;
  /**
   * Timer used to animate the text.
   */
  Timer timer;
  /**
   * Reset position for the vram pointer on page clear.
   */
  uint8_t *origin;
  /**
   * VRAM location where the next character will be printed.
   */
  uint8_t *vram;
  /**
   * Text currently being printed.
   */
  const char *text;
} TextWriter;

/**
 * Core text writer module. Used to animate the drawing of characters to the
 * screen.
 */
extern TextWriter text_writer;


inline bool text_writer_done(void) {
  return text_writer.state == TEXT_WRITER_DONE;
}

#endif
