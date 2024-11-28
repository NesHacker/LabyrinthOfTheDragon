#include <gb/gb.h>
#include "data.h"

/**
 * Safely copies data from the given bank and data address into the given
 * destination address.
 * @param bank Id of the bank from which to load the data.
 * @param src Pointer to source data for the tiles.
 * @param dst Pointer to the destination for the copy.
 * @param n Number of tiles to copy.
 */
void load_tiles(uint8_t bank, uint8_t *src, uint8_t *dst, uint8_t n) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(bank);
  for (uint8_t t = 0; t < n; t++)
    for (uint8_t c = 0; c < 16; c++)
      *dst++ = *src++;
  SWITCH_ROM(_prev_bank);
}

/**
 * Loads a page of 64 tiledata bytes into VRAM at the given destination address.
 * @param bank Id of the bank from which to load the data.
 * @param src Pointer to source data for the tiles.
 * @param dst Pointer to the destination in VRAM.
 */
void load_tile_page(uint8_t bank, uint8_t *src, uint8_t *dst) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(bank);
  for (uint8_t t = 0; t < 0x80; t++)
    for (uint8_t c = 0; c < 16; c++)  
      *dst++ = *src++;
  SWITCH_ROM(_prev_bank);
}

/**
 * Loads a given number of tiles from a tile + attr source into the given memory
 * location in vram.
 * @param b Bank from which to load the data.
 * @param src Source data for the tilemap.
 * @param dst Pointer to the destination for the copy.
 * @param n Number of tile bytes to copy.
 */
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

/**
 * Loads a 20x18 tile + attr screen onto the background.
 * @param b Bank from which to load the data.
 * @param src Pointer to the data to load.
 */
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

/**
 * Loads a full 32x32 tile+attr map.
 * @param b Bank where the source data resides.
 * @param src Pointer to the first byte of the source data.
 * @param dst Pointer (in VRAM) where the data should be written.
 */
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
