/**
 * Fixed bank data loading utility routines.
 */

#ifndef _DATA_H
#define _DATA_H

/**
 * Creates a pointer to VRAM at the given offset.
 * @param offset Offset in VRAM for the pointer.
 */
#define VRAM(offset) (void *)(0x8000 + (offset))

/**
 * VRAM address to sprite tile data.
 */
#define VRAM_SPRITE_TILES (void *)0x8000

/**
 * VRAM address to shared bg/sprite tile data.
 */
#define VRAM_SHARED_TILES (void *)0x8800

/**
 * VRAM address to background tile data.
 */
#define VRAM_BG_TILES (void *)0x9000

/**
 * VRAM address for the background tilemap.
 */
#define VRAM_BACKGROUND (void *)0x9800

/**
 * VRAM address for the window tilemap.
 */
#define VRAM_WINDOW (void *)0x9C00

/**
 * Adds a number of page (64 tiles) offsets to the given tile source pointer.
 * @param ptr Source data pointer.
 * @param n Number of pages by which offset.
 */
#define TILE_PAGE(ptr,n) (void *)((ptr) + 0x80 * 16 * ((n)-1))

void load_tilesets(void);

void load_font_tiles(void);
void load_character_tiles(void);
void load_dungeon_tiles(void);
void load_objects_tiles(void);
void load_world_map_tiles(void);

void load_tiles(uint8_t bank, uint8_t *src, uint8_t *dst, uint8_t n);
void load_tile_page(uint8_t bank, uint8_t *src, uint8_t *dst);
void load_tilemap(uint8_t b, uint8_t *src, uint8_t *dst, uint8_t n);
void load_screen(uint8_t b, uint8_t *src);

#endif
