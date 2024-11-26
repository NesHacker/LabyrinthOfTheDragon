/**
 * Binary data loading variables and routines.
 */

#ifndef _DATA_H
#define _DATA_H

#include <gbdk/incbin.h>

INCBIN_EXTERN(tile_data_dungeon)
INCBIN_EXTERN(tile_data_font)
INCBIN_EXTERN(tile_data_hero)
INCBIN_EXTERN(tile_data_objects)
INCBIN_EXTERN(tile_data_world_map)

// Textbox: 20 x 6 x 2 (tiles & attributes)
#define TILEMAP_TEXTBOX_W 20
#define TILEMAP_TEXTBOX_H 6
INCBIN_EXTERN(tilemap_textbox)

// Full screen tilemaps (20x18 tiles, tile+attr pairs)
INCBIN_EXTERN(tilemap_name_entry)
INCBIN_EXTERN(tilemap_title_screen)

#define TILE_PAGE(ptr,n) (void *)((ptr) + 0x80 * 16 * ((n)-1))

/**
 * Loads graphics tiles into VRAM.
 */
void load_tilesets(void);

void load_font_tiles(void);
void load_character_tiles(void);
void load_dungeon_tiles(void);
void load_objects_tiles(void);
void load_world_map_tiles(void);

void load_tilemap(const uint8_t *tilemap, const uint8_t w, const uint8_t h);

#endif
