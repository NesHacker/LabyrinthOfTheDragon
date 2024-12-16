#ifndef _CORE_H
#define _CORE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "data.h"
#include "palette.h"

// TODO Convert existing code to use core routines and structures
// TODO Automatically generate Tileset structures when compiling tile data
// TODO Make all asset compile tools take core structures into account

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
 * Computes an offset to add to a VRAM pointer to place it at the beginning of
 * tile of the next row on the next row of the background or window.
 * @param col Current column where VRAM is pointing.
 */
#define VRAM_ROW_OFFSET(col) (32 - (col))

/**
 * Random Seed to use for the game. If set to 0 then the game will generate a
 * seed value on the title while waiting for the player to begin the game.
 */
#define RANDOM_SEED 100

/**
 * Number of bytes per 8x8 pixel 2BBP tile.
 */
#define BYTES_PER_TILE 16

/**
 * Denotes that a tileset has a full 256 tiles.
 */
#define TILES_256 0

/**
 * Default VRAM location to write font tiles.
 */
#define VRAM_FONT_TILES VRAM_SHARED_TILES

/**
 * Default VRAM location to write battle tiles.
 */
#define VRAM_BATTLE_TILES (void *)0x9300

/**
 * Core tileset structure.
 */
typedef struct Tileset {
  /**
   * Size (in tiles) for the tileset.
   */
  const uint8_t size;
  /**
   * Bank on which the tileset resides.
   */
  const uint8_t bank;
  /**
   * Pointer to the data for the tileset.
   */
  const uint8_t *const data;
} Tileset;

/**
 * Enumeration of all supported tilemap types.
 */
typedef enum TilemapType {
  /**
   * Tilemap contains interleaved tile and attribute pairs (default).
   */
  TILEMAP_TILE_ATTR_PAIRS,
  /**
   * Tilemap contains tile data only.
   */
  TILEMAP_TILES_ONLY,
  /**
   * Tilemap contains all tiles, then all attributes.
   */
  TILEMAP_TILES_THEN_ATTRS,
  /**
   * Tilemap contains attributes only.
   */
  TILEMAP_ATTR_ONLY,
} TilemapType;

/**
 * Core tilemap structure.
 */
typedef struct Tilemap {
  /**
   * Width of the tilemap.
   */
  const uint8_t width;
  /**
   * Height of the tilemap.
   */
  const uint8_t height;
  /**
   * Bank where the data resides.
   */
  const uint8_t bank;
  /**
   * Data for the tilemap.
   */
  const uint8_t *const data;
  /**
   * Structure for the tilemap data.
   */
  const TilemapType type;
} Tilemap;


// TODO Remove this far pointer palette wrapper, palette data is small enough
//      that if you can't fit it in the bank alongside the code you have bigger
//      fish to fry.
/**
 * Represents a banked set of palette data.
 */
typedef struct Palette {
  /**
   * Bank on which the data resides.
   */
  const uint8_t bank;
  /**
   * Color data for the palette.
   */
  const palette_color_t *const data;
} Palette;

/**
 * Core module wrapper.
 */
typedef struct Core {
  /**
   * Loads a tileset to the given destination.
   * @param s Tileset to load.
   * @param dst Destination pointer in VRAM.
   */
  const void (*load_tileset)(Tileset *s, uint8_t *dst);
  /**
   * Loads a number of tiles from a tileset.
   * @param s Tileset to load.
   * @param dst Destination to write tile data.
   * @param o Tile data offset (in tiles).
   * @param n Number of tiles to write.
   */
  const void (*load_tiles)(Tileset *s, uint8_t *dst, uint8_t o, uint8_t n);
  /**
   * Loads the core battle tileset.
   */
  const void (*load_battle_tiles)(void);
  /**
   * Loads the core font tileset.
   */
  const void (*load_font)(void);
  /**
   * Loads the font tiles for the given player class.
   */
  const void (*load_hero_tiles)(uint8_t player_class);
  /**
   * Loads all hero tiles.
   */
  const void (*load_all_heros)(void);
  /**
   * Loads the map objects tileset.
   */
  const void (*load_object_tiles)(void);
  /**
   * Draws a tilemap at the given location.
   * @param m Tilemap to load.
   * @param vram Destination pointer in VRAM.
   */
  const void (*draw_tilemap)(Tilemap s, uint8_t *dst);
  /**
   * Loads the given palette into the background palette set.
   * @param p Palette to load.
   * @param index Starting palette index.
   * @param n Number of palettes to load.
   */
  const void (*load_bg_palette)(Palette p, uint8_t index, uint8_t n);
  /**
   * Loads the given palette into the foreground palette set.
   * @param p Palette to load.
   * @param index Starting palette index.
   * @param n Number of palettes to load.
   */
  const void (*load_sprite_palette)(Palette p, uint8_t index, uint8_t n);
} Core;

/**
 * The core module.
 */
extern const Core core;

#endif