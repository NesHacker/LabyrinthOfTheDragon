#include <gb/gb.h>
#include "data.h"

void load_tiles(uint8_t bank, uint8_t *src, uint8_t *dst, uint8_t n) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(bank);
  for (uint8_t t = 0; t < n; t++)
    for (uint8_t c = 0; c < 16; c++)
      *dst++ = *src++;
  SWITCH_ROM(_prev_bank);
}

void load_tile_page(uint8_t bank, uint8_t *src, uint8_t *dst) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(bank);
  for (uint8_t t = 0; t < 0x80; t++)
    for (uint8_t c = 0; c < 16; c++)
      *dst++ = *src++;
  SWITCH_ROM(_prev_bank);
}

void load_tile_full(uint8_t bank, uint8_t *src, uint8_t *dst) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(bank);

  uint8_t *dst_page2 = dst;

  VBK_REG = VBK_BANK_0;
  for (uint8_t t = 0; t < 0x80; t++)
    for (uint8_t c = 0; c < 16; c++)
      *dst++ = *src++;
  VBK_REG = VBK_BANK_1;
  for (uint8_t t = 0; t < 0x80; t++)
    for (uint8_t c = 0; c < 16; c++)
      *dst_page2++ = *src++;

  SWITCH_ROM(_prev_bank);
}

void load_tilemap(uint8_t b, uint8_t *src, uint8_t *dst, uint8_t n) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(b);
  for (uint8_t t = 0; t < n; t++) {
    VBK_REG = VBK_TILES;
    *dst = *src++;
    VBK_REG = VBK_ATTRIBUTES;
    *dst++ = *src++;
  }
  SWITCH_ROM(_prev_bank);
}

void load_screen(uint8_t b, uint8_t *src) NONBANKED {
  uint8_t *dst = (void *)0x9800;
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(b);
  for (uint8_t y = 0; y < 18; y++) {
    for (uint8_t x = 0; x < 20; x++) {
      VBK_REG = VBK_TILES;
      *dst = *src++;
      VBK_REG = VBK_ATTRIBUTES;
      *dst++ = *src++;
    }
    dst += 12;
  }
  SWITCH_ROM(_prev_bank);
}

void load_full_tilemap(uint8_t b, uint8_t *src, uint8_t *dst) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(b);
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *dst = *src++;
    VBK_REG = VBK_ATTRIBUTES;
    *dst++ = *src++;
  }
  SWITCH_ROM(_prev_bank);
}
