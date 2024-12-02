/**
 * Banked data loading utility routines.
 */

#ifndef _DATA_H
#define _DATA_H

#include <gb/gb.h>
#include <gbdk/incbin.h>

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
 * Computes the background tilemap VRAM address for the given column and row.
 * @param x Column in VRAM.
 * @param y Row in VRAM.
 */
#define VRAM_BACKGROUND_XY(x, y)  (void *)(0x9800 + (x) + 0x20 * (y))

/**
 * Computes the window tilemap VRAM address for the given column and row.
 * @param x Column in VRAM.
 * @param y Row in VRAM.
 */
#define VRAM_WINDOW_XY(x, y)  (void *)(0x9C00 + (x) + 0x20 * (y))

/**
 * Adds a number of page (64 tiles) offsets to the given tile source pointer.
 * @param ptr Source data pointer.
 * @param n Number of pages by which offset.
 */
#define TILE_PAGE(ptr,n) (void *)((ptr) + 0x80 * 16 * ((n)-1))

/**
 * Safely copies data from the given bank and data address into the given
 * destination address.
 * @param bank Id of the bank from which to load the data.
 * @param src Pointer to source data for the tiles.
 * @param dst Pointer to the destination for the copy.
 * @param n Number of tiles to copy.
 */
void load_tiles(uint8_t bank, uint8_t *src, uint8_t *dst, uint8_t n) NONBANKED;

/**
 * Loads a page of 64 tiledata bytes into VRAM at the given destination address.
 * @param bank Id of the bank from which to load the data.
 * @param src Pointer to source data for the tiles.
 * @param dst Pointer to the destination in VRAM.
 */
void load_tile_page(uint8_t bank, uint8_t *src, uint8_t *dst) NONBANKED;

/**
 * Loads the first page of 64 tile data bytes into the first bank and the next
 * 64 tile data bytes into second bank of the given VRAM destination.
 * @param bank Id of the bank from which to load the data.
 * @param src Pointer to the source data for the tiles.
 * @param dst Pointer to the destination in VRAM.
 */
void load_tile_full(uint8_t bank, uint8_t *src, uint8_t *dst) NONBANKED;

/**
 * Loads a given number of tiles from a tile + attr source into the given memory
 * location in vram.
 * @param b Bank from which to load the data.
 * @param src Source data for the tilemap.
 * @param dst Pointer to the destination for the copy.
 * @param n Number of tile bytes to copy.
 */
void load_tilemap(uint8_t b, uint8_t *src, uint8_t *dst, uint8_t n) NONBANKED;

/**
 * Loads a 20x18 tile + attr screen onto the background.
 * @param b Bank from which to load the data.
 * @param src Pointer to the data to load.
 */
void load_screen(uint8_t b, uint8_t *src);

/**
 * Loads a full 32x32 tile+attr map.
 * @param b Bank where the source data resides.
 * @param src Pointer to the first byte of the source data.
 * @param dst Pointer (in VRAM) where the data should be written.
 */
void load_full_tilemap(uint8_t b, uint8_t *src, uint8_t *dst) NONBANKED;

// Banked data externs
INCBIN_EXTERN(tile_data_dungeon)
INCBIN_EXTERN(tile_data_font)
INCBIN_EXTERN(tile_data_hero)
INCBIN_EXTERN(tile_data_objects)
INCBIN_EXTERN(tile_data_world_map)

INCBIN_EXTERN(tilemap_textbox) 
INCBIN_EXTERN(tilemap_title_screen)
INCBIN_EXTERN(tilemap_name_entry)
INCBIN_EXTERN(tilemap_save_select)

INCBIN_EXTERN(map_example_0)
INCBIN_EXTERN(map_example_1)
#endif
