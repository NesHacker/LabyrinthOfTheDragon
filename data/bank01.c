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

// 6KB of 16KB remaining

INCBIN(tilemap_name_entry, "res/tilemaps/name_entry.tilemap")
INCBIN(tilemap_save_select, "res/tilemaps/save_select.tilemap")
INCBIN(tilemap_title_screen, "res/tilemaps/title_screen.tilemap")
INCBIN(tilemap_textbox, "res/tilemaps/textbox.tilemap")
