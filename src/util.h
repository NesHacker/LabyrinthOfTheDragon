/**
 * GB/GBC common utility types, functions, and macros.
 */
#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>

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

// Function prototypes
void clear_background(void);
void draw_text(uint8_t *vram, char *text, uint8_t attr);

#endif
