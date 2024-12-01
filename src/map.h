/*
 * Map.h - general structures and functions for handling the world map state.
 * 
 * The world of the game is represeted as a series of maps, with each map having
 * at least one page.
 * 
 * Pages are a series of bytes representing a 16px by 16px tile on the screen,
 * with each page having 256 = 16 columns by 16 rows of tile data.
 * 
 * Each tile in a page is represented by a byte containing a 6-bit tile number
 * (which is used to lookup graphics data loaded into VRAM) along with a 2-bit
 * tile attribute (see enum MapAttribute below).
 */

#ifndef _MAP_H
#define _MAP_H

#include <gb/gb.h>
#include <stdint.h>

/**
 * Main state enumaration for the world map play controller.
 */
typedef enum MapState {
  /**
   * Denotes that the player is standing still and the world map controller is
   * awaiting input.
   */
  MAP_STATE_WAITING,
  /**
   * Denotes that the player is moving within the map.
   */
  MAP_STATE_MOVING,
} MapState;

/**
 * Enumeration of all types of map exit tiles.
 */
typedef enum ExitType {
  /**
   * Normal doorway exit.
   */
  EXIT_DOOR,
  /**
   * Stairs exit (down or up).
   */
  EXIT_STAIRS,
  /**
   * A hole in the ground (fall down).
   */
  EXIT_HOLE,
  /**
   * A wall ladder (down or up).
   */
  EXIT_LADDER,
  /**
   * A magical portal.
   */
  EXIT_PORTAL,
} ExitType;

/**
 * Represents a map exit and provides information about the destination for the
 * exit.
 */
typedef struct MapExit {
  /**
   * Page where the exit tile resides.
   */
  uint8_t page_id;
  /**
   * Column of the exit.
   */
  uint8_t col;
  /**
   * Row of the exit.
   */
  uint8_t row;
  /**
   * The destination map id.
   */
  uint8_t to_map_id;
  /**
   * The page id on the destination map.
   */
  uint8_t to_page_id;
  /**
   * Column to place the character on the destination page.
   */
  uint8_t to_col;
  /**
   * Row to place the character on the destination page.
   */
  uint8_t to_row;
} MapExit;

/**
 * Holds the bank and data pointer for a map page.
 */
typedef struct MapPage {
  uint8_t bank;
  uint8_t *data;
} MapPage;

/**
 * Contains callbacks and data for a game map.
 */
typedef struct Map {
  /**
   * Default starting column for the player if the map is loaded directly and
   * not via an exit from another map.
   */
  uint8_t default_start_column;
  
  /**
   * Default starting row for the player if the map is loaded directly and not
   * via an exit from another map.
   */
  uint8_t default_start_row;

  /**
   * List of all pages for the map.
   */
  MapPage *pages;

  /**
   * Number of pages in the map.
   */
  uint8_t num_pages;

  /**
   * List of exits for all pages on the map.
   */
  //MapExit *exits;

  /**
   * Bank for the map's tile data.
   */
  uint8_t tile_bank;
  
  /**
   * Pointer to a 16x16 (256 tile) page of tile data that is used for the map.
   */
  uint8_t *tile_data;
  
  /**
   * Bank where the map's callback functions reside.
   */
  uint8_t callback_bank;
  
  /**
   * Called when the map is initialized by the game engine.
   */
  void (*on_init)(void);
  
  /**
   * Called on game loop update when the map is active.
   */
  void (*on_update)(void);
  
  /**
   * Called on VBLANK draw when the map is active.
   */
  void (*on_draw)(void);
  
  /**
   * Called when the player enters the map from another map.
   * @param n Number for the exit/entrace from which the player entered.
   */
  void (*on_enter)(uint8_t n);
  
  /**
   * Called when the map is active and the player moves into an "exit" tile.
   * @param col_row The packed column & row for the exit.
   */
  void (*on_exit)(uint8_t col, uint8_t row);
  
  /**
   * Called when the map is active and the player moves into a floor tile.
   * @param col_row The packed column & row for the tile.
   */
  void (*on_move)(uint8_t col, uint8_t row);

  /**
   * Called when the map is active and the player moves into a special tile.
   */
  void (*on_special)(uint8_t col, uint8_t row);
} Map;

/**
 * Mask used to isolate a map data byte's "tile id".
 */
#define MAP_TILE_MASK 0b00111111

/**
 * Mask used to isolate a map data byte's "attribute".
 */
#define MAP_ATTR_MASK 0b11000000

/**
 * Enumeration of map attribute values. Map attributes are used by the engine to
 * handle common / high level interactions for map movement in addition to 
 * determining specific map callbacks to execute during game logic.
 */
typedef enum MapTileAttribute {
  /**
   * Denotes an impassible wall.
   */
  MAP_WALL,
  /**
   * Denotes a passable ground tile.
   */
  MAP_GROUND,
  /**
   * Denotes an exit tile used to load other map / pages.
   */
  MAP_EXIT,
  /**
   * Denotes a special tile to be handled by a map's code implementation.
   */
  MAP_SPECIAL
} MapTileAttribute;

/**
 * Pointer to the map that is currently loaded.
 */
extern Map *map_current;

/**
 * Id of the page that is loaded on the map.
 */
extern uint8_t map_page;

/**
 * A 16 x 16 (256 entry) array that contains the tile attribute type for every
 * tile in the currently loaded page.
 */
extern MapTileAttribute map_page_attributes[256];

/**
 * Initialize the world map state.
 */
void init_map(void) NONBANKED;

/**
 * Game loop update routine for the world map state.
 */
void update_map(void) NONBANKED;

/**
 * VBLANK draw routine for the world map state.
 */
void draw_map(void) NONBANKED;

#endif