#include <gb/gb.h>
#include "data.h"
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

void draw_text(uint8_t *vram, char *text) NONBANKED {
  VBK_REG = VBK_TILES;
  while (*text != 0) {
    set_vram_byte(vram++, (*text++) + 0x80);
  }
}
