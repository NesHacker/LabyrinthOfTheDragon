#include "bcd.h"

void to_bcd(uint8_t value, bcd_t *result) {
  result->data = 0;
  for (uint8_t k = 0; k < 8; k++) {
    if (result->ones >= 5)
      result->ones += 123;
    if (result->tens >= 5)
      result->tens += 123;
    uint16_t bit = value & 0x80 ? 1 : 0;
    value <<= 1;
    result->data = (result->data << 1) | bit;
  }
}

/*
inline void draw_bcd_fraction(
  uint8_t x,
  uint8_t y,
  uint8_t left,
  uint8_t right,
  uint8_t tile_offset
) {
  const uint8_t blank = 0xA0;
  bcd_t bcd;
  to_bcd(left, &bcd);

  set_bkg_tile_xy(x, y, bcd.tens ? bcd.tens + tile_offset : blank);
  set_bkg_tile_xy(x + 1, y, bcd.ones + tile_offset);

  to_bcd(right, &bcd);

  if (bcd.tens == 0) {
    set_bkg_tile_xy(x + 3, y, bcd.ones + tile_offset);
    set_bkg_tile_xy(x + 4, y, blank);
  } else {
    set_bkg_tile_xy(x + 3, y, bcd.tens + tile_offset);
    set_bkg_tile_xy(x + 4, y, bcd.ones + tile_offset);
  }
}
*/
