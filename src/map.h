#ifndef _MAP_H
#define _MAP_H

#include <gb/gb.h>
#include <stdbool.h>
#include <stdint.h>

#include "core.h"
#include "textbox.h"

/**
 * Bank in which the map system resides.
 */
#define MAP_SYSTEM_BANK 2

/**
 * State animation state for the overworld hero.
 */
typedef enum HeroState {
  HERO_STILL,
  HERO_WALKING,
} HeroState;

/**
 * Main state enumaration for the world map controller.
 */
typedef enum MapState {
  /**
   * Denotes that the map is currently inactive and should perform no updates.
   */
  MAP_STATE_INACTIVE,
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
  MAP_STATE_TEXTBOX,
  /**
   * Map is being transitioned to the battle system.
   */
  MAP_STATE_START_BATTLE,
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
  GameRomBank bank;
  /**
   * Pointer to the data for the map.
   */
  const uint8_t *data;
  /**
   * Width for the map (in map tiles).
   */
  uint8_t width;
  /**
   * Height for the map (in map tiles).
   */
  uint8_t height;
} Map;

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
 * Data representation of a treasure chest in an area. Chests can be placed at
 * any position in any map. Whether or not they have been opened is determined
 * by a global flag (so the state can be saved to SRAM).
 */
typedef struct Chest {
  /**
   * Id for the chest. This should be the chest list array index.
   */
  uint8_t id;
  /**
   * Id for the map where the chest resides.
   */
  uint8_t map_id;
  /**
   * Column for the chest.
   */
  uint8_t col;
  /**
   * Row for the chest.
   */
  uint8_t row;
  /**
   * Flag page for the chest's "open state".
   */
  FlagPage flag_page;
  /**
   * Flag mask for the "opened" state.
   */
  uint8_t open_flag;
} Chest;

/**
 * Defines a floor (level) for the game.
 */
typedef struct Floor {
  /**
   * Unique id for the area. Must be non-zero.
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
   * Palettes to use for the dungeon.
   */
  palette_color_t *palettes;
  /**
   * Number of pages in the map.
   */
  uint8_t num_maps;
  /**
   * List of all pages for the map.
   */
  Map *maps;
  /**
   * Number of exit entries for the area.
   */
  uint8_t num_exits;
  /**
   * List of exits for all pages on the map.
   */
  Exit *exits;
  /**
   * Number of chests in the area.
   */
  uint8_t num_chests;
  /**
   * Array of treasure chests for the area.
   */
  Chest *chests;
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
   * Called before a chest is opened.
   * @param chest_id Id of the chest being opened.
   * @return `true` if the chest can be opened.
   */
  bool (*before_chest)(Chest *chest);
  /**
   * Called after a chest has been opened.
   */
  void (*on_chest)(Chest *chest);
  /**
   * Called when the player enters a map from another map.
   * @param from_id Id of the map the player exited.
   * @param to_id Id of the map the player is entering.
   */
  void (*on_enter)(uint8_t from_id, uint8_t to_id);
  /**
   * Called when the map is active and the player moves into a special tile.
   * @return `true` if the map should prevent default behavior.
   */
  bool (*on_special)(void);
  /**
   * Called when the map is active and the player moves into an "exit" tile.
   * @return `true` if the map should prevent default behavior.
   */
  bool (*on_exit)(void);
  /**
   * Called when the map is active and the player moves into a floor tile.
   * @param col The column for the tile.
   * @param row The row for the tile.
   * @return `true` if the map should prevent default behavior.
   */
  bool (*on_move)(void);
} Floor;

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
 * Pointer to the active area.
 */
extern Floor *active_area;

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

// TODO Factor me out in to a struct
extern HeroState hero_state;
extern Direction hero_direction;
extern uint8_t hero_x;
extern uint8_t hero_y;













// TODO Clean this mess up!


#define MAP_HORIZ_LOADS 12
#define MAP_VERT_LOADS 11

typedef struct MapTile {
  bool blank;
  uint8_t tile;
  uint8_t attr;
  uint8_t map_attr;
} MapTile;

typedef struct MapData {
  GameRomBank bank;
  uint8_t *data;
  int8_t width;
  int8_t height;
} MapData;

typedef struct MapSystem {
  // Global State
  MapState state;

  // Map position
  int8_t x;
  int8_t y;

  // Movement
  int8_t scroll_x;
  int8_t scroll_y;
  Direction move_direction;
  uint8_t move_step;

  // Progressive load
  int8_t vram_x;
  int8_t vram_y;
  MapTile tile_buf[2 * MAP_HORIZ_LOADS];
  uint8_t buffer_pos;
  uint8_t buffer_max;
  int8_t vram_col;
  int8_t vram_row;
  int8_t vram_d_col;
  int8_t vram_d_row;

  // Map data
  const Map *active_map;
} MapSystem;



extern MapSystem map;




/**
 * Sets the position of the active map. Note this method will not re-render the
 * map based on the position set. You must call `reset_map_screen()` to do this.
 * @param x Horizontal position.
 * @param y Vertical position.
 */
inline void set_map_position(int8_t x, int8_t y) {
  map.x = x;
  map.y = y;
}















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
 * Set the "open" state graphics for the given chest.
 * @param chest Chest that needs to be graphically depicted as "open".
 */
void set_chest_open_graphics(Chest *chest);

/**
 * Initiates battle using the currently configured encounter. This should be
 * called by area handlers to start battle after initalizing the `encounter`
 * state used by the battle system.
 */
void start_battle(void);

/**
 * Called when returning to the map system from the battle system.
 */
void return_from_battle(void) NONBANKED;

inline void set_active_map(const Map *m) NONBANKED {
  map.active_map = m;
}

/**
 * Opens a textbox while on the world map.
 * @param text Text to display in the text box.
 */
inline void map_textbox(const char *text) {
  map_state = MAP_STATE_TEXTBOX;
  textbox.open(text);
}

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
 * @param c Column to check.
 * @param r Row to check.
 * @return `true` If the player is at the given column and row in the map.
 */
inline bool player_at(uint8_t c, uint8_t r) {
  return map_col == c && map_row == r;
}

/**
 * @param col Column to check.
 * @param row Row to check.
 * @param d Direction to check.
 * @return `true` If the player is at the given position in the map and is
 *   facing the given direction.
 */
inline bool player_at_facing(uint8_t col, uint8_t row, Direction d) {
  return map_col == col && map_row == row && hero_direction == d;
}

/**
 * @param column Column of the target square.
 * @param row Row of the target square.
 * @return `true` If the player is facing the target square.
 */
inline bool player_facing(uint8_t col, uint8_t row) {
  return (
    player_at_facing(col - 1, row, RIGHT) ||
    player_at_facing(col + 1, row, LEFT) ||
    player_at_facing(col, row - 1, DOWN) ||
    player_at_facing(col, row + 1, UP)
  );
}

/**
 * Executes the active area's `on_init` callback if one is set.
 */
inline void on_init(void) {
  if (active_area->on_init)
    active_area->on_init();
}

/**
 * Executes the active area's `on_update` callback if one is set.
 */
inline void on_update(void) {
  if (active_area->on_update)
    active_area->on_update();
}

/**
 * Executes the active area's `on_draw` callback if one is set.
 */
inline void on_draw(void) {
  if (active_area->on_draw)
    active_area->on_draw();
}

/**
 * Executes the active area's `on_action` callback if one is set.
 */
inline void on_action(void) {
  if (active_area->on_action)
    active_area->on_action();
}

/**
 * Executes the active area's `before_chest` callback if one is set.
 */
inline bool before_chest(Chest *chest) {
  if (active_area->before_chest)
    return active_area->before_chest(chest);
  return true;
}

/**
 * Executes the active area's `on_chest` callback if one is set.
 */
inline void on_chest(Chest *chest) {
  if (active_area->on_chest)
    active_area->on_chest(chest);
}

/**
 * Executes the active area's `on_enter` callback if one is set.
 */
inline void on_enter(uint8_t from_id, uint8_t to_id) {
  if (active_area->on_enter)
    active_area->on_enter(from_id, to_id);
}

/**
 * Executes the active area's `on_special` callback if one is set.
 */
inline bool on_special(void) {
  if (active_area->on_special)
    return active_area->on_special();
  return false;
}

/**
 * Executes the active area's `on_exit` callback if one is set.
 */
inline bool on_exit(void) {
  if (active_area->on_exit)
    return active_area->on_exit();
  return false;
}

/**
 * Executes the active area's `on_move` callback if one is set.
 */
inline bool on_move(void) {
  if (active_area->on_move)
    return active_area->on_move();
  return false;
}

// External area data references
extern Floor area0;

#endif
