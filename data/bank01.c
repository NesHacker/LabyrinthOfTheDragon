#pragma bank 1

#include <gb/gb.h>
#include <gbdk/incbin.h>

// 2KB Core font tiles
INCBIN(tile_data_font, "res/tiles/font.bin")

// 2KB Hero sprites
INCBIN(tile_data_hero, "res/tiles/hero.bin")

// 2KB World Objects
INCBIN(tile_data_objects, "res/tiles/objects.bin")

// 4K Dungeon tiles
INCBIN(tile_data_dungeon, "res/tiles/dungeon.bin")

// 6KB of 16KB remaining
