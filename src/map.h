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
#include <stdbool.h>
#include <stdint.h>

#include "hero.h"
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
  /**
   * Denotes that the graphics are fading out for a progressive map/area load.
   */
  MAP_STATE_FADE_OUT,
  /**
   * Denotes that the graphics are fading back in after a progessive map/area
   * load.
   */
  MAP_STATE_FADE_IN,
  /**
   * Denotes that the current map is being loaded.
   */
  MAP_STATE_LOAD,
  /**
   * Denotes that the textbox is currently displayed.
   */
  MAP_STATE_TEXTBOX
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
  /**
   * Direction the hero should face after taking an exit.
   */
  Direction heading;
  /**
   * Type of exit.
   */
  ExitType exit;
} Exit;

/**
 * Holds the bank and data pointer for a map page.
 */
typedef struct Map {
  /**
   * Id for the map. This must be the map's index in it's area's maps table.
   */
  uint8_t id;

  /**
   * Bank where the data for the map resides.
   */
  uint8_t bank;

  /**
   * Pointer to the data for the map.
   */
  uint8_t *data;
} Map;

/**
 * Contains callbacks and data for a game area. Areas define tilesets, palettes,
 * maps, and callbacks used by the world map controller to produce adventure
 * gameplay for the game.
 */
typedef struct Area {
  /**
   * Id for the area. This should be the same as its index in the areas
   * table.
   */
  uint8_t id;

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
  uint8_t num_maps;

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
  Exit *exits;

  /**
   * Number of exit entries for the area.
   */
  uint8_t num_exits;

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
   * Called when the player interacts by pressing the "A" button while in the
   * area.
   * @param map_id Id of the currently loaded map.
   * @param col Current column for the player.
   * @param row Current row for the player.
   */
  void (*on_action)(void);

  /**
   * Called when the player enters the map from another map.
   * @param from_id Id of the map the player exited.
   * @param to_id Id of the map the player is entering.
   */
  void (*on_enter)(uint8_t from_id, uint8_t to_id);

  /**
   * Called when the map is active and the player moves into a special tile.
   */
  void (*on_special)(uint8_t col, uint8_t row);

  /**
   * Called when the map is active and the player moves into an "exit" tile.
   * @param col_row The packed column & row for the exit.
   */
  void (*on_exit)(void);

  /**
   * Called when the map is active and the player moves into a floor tile.
   * @param col_row The packed column & row for the tile.
   */
  void (*on_move)(uint8_t col, uint8_t row);
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
extern Map *active_map;

/**
 * Id of the currently loaded map.
 */
extern uint8_t current_map_id;

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
 * State of the world map controller.
 */
extern MapState map_state;

/**
 * Absolute x position of the hero on the map.
 */
extern uint8_t map_x;

/**
 * Absolute y position of the hero on the map.
 */
extern uint8_t map_y;

/**
 * Horizontal background scroll for the current map.
 */
extern uint8_t map_scroll_x;

/**
 * Vertical background scroll for the current map.
 */
extern uint8_t map_scroll_y;

/**
 * Direction the map is currently moving.
 */
extern Direction map_move_direction;

/**
 * Frame counter for the current map move.
 */
extern uint8_t map_move_counter;

/**
 * Reference to the exit taken prior to a progressive load. Used to initialize
 * player's sprite and such after the transition.
 */
extern Exit *current_exit;

/**
 * Opens a textbox while on the world map.
 * @param text Text to display in the text box.
 */
inline void map_textbox(const char *text);

/**
 * Initialize the world map controller.
 */
void init_world_map(void) NONBANKED;

/**
 * Update callback for the world map controller.
 */
void update_world_map(void) NONBANKED;

/**
 * VBLANK draw routine for the world map controller.
 */
void draw_world_map(void) NONBANKED;

/**
 * @return The map tile attribute at the player's current column and row.
 */
inline MapTileAttribute get_tile_attribute_xy(uint8_t col, uint8_t row) {
  return map_tile_attributes[col + row * 16];
}

/**
 * @param col Column for the tile attribute.
 * @param row Row for the tile attribute.
 * @return The map tile attribute at the given column and row.
 */
inline MapTileAttribute get_tile_attribute(void) {
  return map_tile_attributes[map_col + map_row * 16];
}

/**
 * @param d Direction to check.
 * @return The map tile attribute at the given direction.
 */
inline MapTileAttribute get_tile_attribute_at(Direction d) {
  switch (d) {
  case NO_DIRECTION:
    return get_tile_attribute();
  case UP:
    return get_tile_attribute_xy(map_col, map_row - 1);
  case DOWN:
    return get_tile_attribute_xy(map_col, map_row + 1);
  case LEFT:
    return get_tile_attribute_xy(map_col - 1, map_row);
  default:
    return get_tile_attribute_xy(map_col + 1, map_row);
  }
}

/**
 * @param id Id of the map to check.
 * @return `true` if the map with the given id is active.
 */
inline bool is_map(uint8_t id) {
  return active_map->id == id;
}

/**
 * @param col Column to check.
 * @param row Row to check.
 * @param d Direction to check.
 * @return `true` If the player is at the given position in the map and is
 *   facing the given direction.
 */
inline bool player_at(uint8_t col, uint8_t row, Direction d) {
  return map_col == col && map_row == row && hero_direction == d;
}

#endif