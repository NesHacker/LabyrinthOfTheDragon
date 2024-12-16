#include <gb/gb.h>

#include "core.h"
#include "bcd.h"
#include "util.h"

void clear_background(void) NONBANKED {
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *vram = 0;
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = 0;
  }
}

void fill_background(uint8_t tile_id, uint8_t attr) NONBANKED {
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *vram = tile_id;
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = attr;
  }
}

void draw_text(uint8_t *vram, const char *text, uint8_t max) NONBANKED {
  VBK_REG = VBK_TILES;
  while (*text != 0 && max) {
    set_vram_byte(vram++, (*text++) + FONT_OFFSET);
    max--;
  }
  while (max) {
    set_vram_byte(vram++, FONT_SPACE);
    max--;
  }
}

void print_fraction(uint8_t *vram, uint16_t n, uint16_t d) {
  bcd_t left, right;
  to_bcd16(n, &left);
  to_bcd16(d, &right);

  VBK_REG = VBK_TILES;

  if (left.d2) {
    set_vram_byte(vram++, left.d2 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, left.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, left.d0 + FONT_DIGIT_OFFSET);
  } else if (!left.d2 && left.d1) {
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, left.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, left.d0 + FONT_DIGIT_OFFSET);
  } else {
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, left.d0 + FONT_DIGIT_OFFSET);
  }

  set_vram_byte(vram++, FONT_SLASH);

  if (right.d2) {
    set_vram_byte(vram++, right.d2 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, right.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, right.d0 + FONT_DIGIT_OFFSET);
  } else if (!right.d2 && right.d1) {
    set_vram_byte(vram++, right.d1 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, right.d0 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, FONT_SPACE);
  } else {
    set_vram_byte(vram++, right.d0 + FONT_DIGIT_OFFSET);
    set_vram_byte(vram++, FONT_SPACE);
    set_vram_byte(vram++, FONT_SPACE);
  }
}
