/**
 * GB/GBC common utility types, functions, and macros.
 */
#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>

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
 * Prints a fraction to the screen at the given vram location.
 * @param vram Vram location to print the fraction.
 * @param n Numerator for the fraction.
 * @param d Denominator for the fraction.
 */
void print_fraction(uint8_t *vram, uint16_t n, uint16_t d);

#endif
