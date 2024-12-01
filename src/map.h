/*
 * Map.h - general structures and functions for handling the world map state.
 * 
 * The game world is represented by a series of areas, with each area having
 * at least one 16x16 tile map that fills an entire background tilemap. Maps are
 * connected to one another (both within an area, and across areas) via exits
 * and can extensively scripted using function pointer callbacks.
 */

#ifndef _MAP_H
#define _MAP_H

#include <gb/gb.h>
#include <stdint.h>
#include "util.h"

/**
 * Main state enumaration for the world map controller.
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
typedef struct Exit {
  /**
   * Map where the exit tile resides.
   */
  uint8_t map_id;
  /**
   * Column of the exit.
   */
  uint8_t col;
  /**
   * Row of the exit.
   */
  uint8_t row;
  /**
   * The destination area id.
   */
  uint8_t to_area;
  /**
   * The destination map id.
   */
  uint8_t to_map;
  /**
   * Column to place the character on the destination page.
   */
  uint8_t to_col;
  /**
   * Row to place the character on the destination page.
   */
  uint8_t to_row;
} Exit;

/**
 * Holds the bank and data pointer for a map page.
 */
typedef struct Map {
  uint8_t bank;
  uint8_t *data;
} Map;

/**
 * Contains callbacks and data for a game area. Areas define tilesets, palettes,
 * maps, and callbacks used by the world map controller to produce adventure
 * gameplay for the game.
 */
typedef struct Area {
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
  Map *maps;

  /**
   * Number of pages in the map.
   */
  uint8_t num_pages;

  /**
   * Bank for the map's tile data.
   */
  uint8_t tile_bank;
  
  /**
   * Pointer to the background tile data for the map.
   */
  uint8_t *bg_tile_data;
  
  /**
   * Palettes to use for the dungeon.
   */
  uint16_t *palettes;
  
  /**
   * List of exits for all pages on the map.
   */
  // TODO Handle exits
  // MapExit *exits;

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
} Area;

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
 * Pointer to the area that is currently loaded.
 */
extern Area *current_area;

/**
 * Id of the currently loaded map.
 */
extern uint8_t current_map;

/**
 * A 16 x 16 (256 entry) array that contains the tile attribute type for every
 * tile in the current map.
 */
extern MapTileAttribute map_tile_attributes[256];

/**
 * Column the player occupies in the active map.
 */
extern uint8_t map_col;

/**
 * Row the player occupies in the active map.
 */
extern uint8_t map_row;

/**
 * Initialize the world map controller.
 */
void init_map(void) NONBANKED;

/**
 * Update callback for the world map controller.
 */
void update_map(void) NONBANKED;

/**
 * VBLANK draw routine for the world map controller.
 */
void draw_map(void) NONBANKED;

#endif