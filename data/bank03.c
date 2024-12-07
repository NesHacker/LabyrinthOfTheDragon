#pragma bank 3

#include <gb/gb.h>
#include <gbdk/incbin.h>

// Beholder Tiles (1568 = 7x7x2 * 16 Bytes)
INCBIN(tile_beholder, "res/tiles/beholder.bin")

// Kobold Tiles (1568 = 7x7x2 * 16 Bytes)
INCBIN(tile_kobold, "res/tiles/kobold.bin")

// TOTAL = 3136 bytes / 16384 bytes
