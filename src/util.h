/**
 * GB/GBC common utility types, functions, and macros.
 */
#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>

/**
 * Tile offset where the font tiles begin.
 */
#define FONT_OFFSET 0x80

/**
 * Tile id for the '0' character with backing when the font tileset is loaded.
 */
#define FONT_DIGIT_OFFSET 0xB0

/**
 * @return The tile id for the given digit.
 * @param d Digit to convert.
 */
#define FONT_DIGIT(d) ((d) + FONT_DIGIT_OFFSET)

/**
 * Tile id for a space when the font tileset is loaded.
 */
#define FONT_SPACE 0xA0

/**
 * Tile id for a '/' character when the font tileset is loaded.
 */
#define FONT_SLASH 0xAF

/**
 * Tile id for a horizontal menu border (top normal, flip-y for bottom).
 */
#define FONT_BORDER_TOP 0x91

/**
 * Universal direction type.
 */
typedef enum Direction {
  NO_DIRECTION, DOWN, UP, LEFT, RIGHT
} Direction;

/**
 * Basic timer structure for handling frame count timers.
 */
typedef struct Timer {
  uint8_t counter;
  uint8_t duration;
} Timer;

/**
 * Initializes a timer with the given duration.
 * @param t The timer to initialize.
 * @param d The duration for the timer.
 */
#define init_timer(t, d) do {\
    (t).duration = d; \
    (t).counter = d; \
  } while (0)

/**
 * Updates the timer for this frame.
 * @return `true` if the timer has completed.
 * @example
 * if (update_timer(my_timer)) {
 *   // Handle trigger code...
 *   reset_timer(my_timer)
 * }
 */
#define update_timer(t) ((--(t).counter) == 0)

/**
 * Resets a timer to let it count down again.
 */
#define reset_timer(t) (t).counter = (t).duration

/**
 * Disables interrupts, turns of the LCD.
 */
#define lcd_off() do { \
    disable_interrupts(); \
    DISPLAY_OFF; \
  } while (0)

/**
 * Enables interrupts and turns the LCD on.
 */
#define lcd_on() do { \
    DISPLAY_ON; \
    enable_interrupts(); \
  } while (0)

/**
 * Defines a uint16_t* external for a binary include.
 */
#define INCBIN_EXTERN_16(VARNAME)  extern const uint16_t VARNAME[]; \
  extern const void __size_ ## VARNAME; \
  extern const void __bank_ ## VARNAME;

/**
 * Clears the 32x32 tile background and fills tiles and attribute bytes with 0s.
 */
void clear_background(void);

/**
 * Fill the background with the given tile and attribute.
 * @param tile Tile to fill.
 * @param attr Attribute to fill.
 */
void fill_background(uint8_t tile, uint8_t attr);

/**
 * Draws text to the given VRAM location.
 * @param vram Pointer to VRAM where the text tiles should be written.
 * @param text The text to write.
 * @param max Maximum length for the text. Clears unused characters with spaces.
 */
void draw_text(uint8_t *vram, const char *text, uint8_t max);

/**
 * Draws a preformatted string to the given VRAM location.
 * @param vram Pointer to VRAM where the text tiles should be written.
 * @param string The text to write.
 * @param max Maximum length for the text. Clears unused characters with spaces.
 */
void draw_string(uint8_t *vram, const char *string, uint8_t max);

#endif
