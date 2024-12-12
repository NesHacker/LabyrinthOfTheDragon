#pragma bank 4

#include <gb/gb.h>
#include <gbdk/incbin.h>

// Kobold Tiles (1568 = 7x7x2 * 16 Bytes)
INCBIN(tile_kobold, "res/tiles/kobold.bin")

// Beholder Tiles (1568 = 7x7x2 * 16 Bytes)
INCBIN(tile_beholder, "res/tiles/beholder.bin")

// TOTAL = 3136 bytes / 16384 bytes
