#pragma bank 1

#include <gb/gb.h>
#include <gbdk/incbin.h>

// 2KB Core font tiles
INCBIN(tile_data_font, "res/tiles/font.bin")

// Battle Icons
INCBIN(tile_battle, "res/tiles/battle.bin")

// 2KB Hero sprites
INCBIN(tile_data_hero, "res/tiles/hero.bin")

// 2KB World Objects
INCBIN(tile_data_objects, "res/tiles/objects.bin")

// 4K Dungeon tiles
INCBIN(tile_data_dungeon, "res/tiles/dungeon.bin")

// Beholder Tiles (2352 bytes [7 * 21 * 16])
INCBIN(tile_beholder, "res/tiles/beholder.bin")

// 6KB of 16KB remaining
