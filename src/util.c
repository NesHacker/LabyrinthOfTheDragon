#include <gb/gb.h>
#include "data.h"
#include "util.h"

/**
 * Clears the 32x32 tile background and fills tiles and attribute bytes with 0s.
 */
void clear_background(void) NONBANKED {
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *vram = 0;
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = 0;
  }
}

/**
 * Draws text to the given VRAM location. Assumes that the text is null
 * null terminated.
 * @param vram Pointer to VRAM where the text tiles should be written.
 * @param text The text to write.
 * @param pal Id of the palette to use for the text tiles.
 */
void draw_text(uint8_t *vram, char *text, uint8_t pal) NONBANKED {
  const uint8_t attr = (pal & 0x03) | 0b00001000;
  while (*text != 0) {
    char c = *text++;
    VBK_REG = VBK_TILES;
    *vram = c + 0x80;  
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = attr;
  }
}
