/**
 * Fixed bank data loading utility routines.
 */

#ifndef _DATA_H
#define _DATA_H

#define TILE_PAGE(ptr,n) (void *)((ptr) + 0x80 * 16 * ((n)-1))

void load_tilesets(void);

void load_font_tiles(void);
void load_character_tiles(void);
void load_dungeon_tiles(void);
void load_objects_tiles(void);
void load_world_map_tiles(void);

void load_tilemap(const uint8_t *tilemap, const uint8_t w, const uint8_t h);

#endif
