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
 * Number of progressive tile loads required when scrolling vertically.
 */
#define MAP_HORIZ_LOADS 12

/**
 * Number of progressive tiles loads required when scroll horizontally.
 */
#define MAP_VERT_LOADS 11

/**
 * Number of tiles by which the hero is offset on the screen horizontally.
 */
#define HERO_X_OFFSET 4

/**
 * Number of tiles by which the hero is offset on the screen vertically.
 */
#define HERO_Y_OFFSET 4

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
 * Represents a tile in a game map.
 */
typedef struct MapTile {
  /**
   * Whether or not the tile is blank. These tiles are used when rendering
   * outside the bounds of the current map. Default tile is 0x00 and default
   * attribute is 0x00.
   */
  bool blank;
  /**
   * Base vram tile id. Used to generate graphics tiles during rendering.
   */
  uint8_t tile;
  /**
   * VRAM attribute. This is set for every tile rendered from the base tile.
   */
  uint8_t attr;
  /**
   * Map game data attribute. Holds high level information about the tile (is
   * it a wall, etc.).
   */
  MapTileAttribute map_attr;
} MapTile;

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
  uint8_t *data;
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
 * Use these values when denoting map ids instead of hard coded constants.
 */
typedef enum MapId {
  MAP_A,
  MAP_B,
  MAP_C,
  MAP_D,
  MAP_E,
  MAP_F,
  MAP_G,
  MAP_H,
  MAP_I,
} MapId;

/**
 * Defines a floor (level) for the game.
 */
typedef struct Floor {
  /**
   * Unique id for the area. Must be non-zero.
   */
  uint8_t id;
  /**
   * Index of the starting map for the floor.
   */
  uint8_t default_map;
  /**
   * Default starting column for the player on the starting map.
   */
  uint8_t default_x;
  /**
   * Default starting row for the player on the starting map.
   */
  uint8_t default_y;
  /**
   * Palettes to use for the dungeon.
   */
  const palette_color_t *palettes;
  /**
   * Number of pages in the map.
   */
  uint8_t num_maps;
  /**
   * List of all pages for the map.
   */
  const Map *maps;
  /**
   * Number of exit entries for the area.
   */
  uint8_t num_exits;
  /**
   * List of exits for all pages on the map.
   */
  const Exit *exits;
  /**
   * Number of chests in the area.
   */
  uint8_t num_chests;
  /**
   * Array of treasure chests for the area.
   */
  const Chest *chests;
  /**
   * Called when the map is initialized by the game engine.
   */
  const void (*on_init)(void);
  /**
   * Called on game loop update when the map is active.
   */
  const void (*on_update)(void);
  /**
   * Called on VBLANK draw when the map is active.
   */
  const void (*on_draw)(void);
  /**
   * Called when the player interacts by pressing the "A" button while in the
   * area.
   * @param map_id Id of the currently loaded map.
   * @param col Current column for the player.
   * @param row Current row for the player.
   */
  const void (*on_action)(void);
  /**
   * Called before a chest is opened.
   * @param chest_id Id of the chest being opened.
   * @return `true` if the chest can be opened.
   */
  const bool (*before_chest)(Chest *chest);
  /**
   * Called after a chest has been opened.
   */
  const void (*on_chest)(Chest *chest);
  /**
   * Called when the player enters a map from another map.
   * @param from_id Id of the map the player exited.
   * @param to_id Id of the map the player is entering.
   */
  const void (*on_enter)(uint8_t from_id, uint8_t to_id);
  /**
   * Called when the map is active and the player moves into a special tile.
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_special)(void);
  /**
   * Called when the map is active and the player moves into an "exit" tile.
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_exit)(void);
  /**
   * Called when the map is active and the player moves into a floor tile.
   * @param col The column for the tile.
   * @param row The row for the tile.
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_move)(void);
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
 * Map system main state. Holds all global memory values used in the system.
 */
typedef struct MapSystem {
  /**
   * Top-level state for the map system.
   */
  MapState state;
  /**
   * Horizontal map coordinate.
   */
  int8_t x;
  /**
   * Veritcal map coordinate.
   */
  int8_t y;
  /**
   * Horizontal background scroll position.
   */
  int8_t scroll_x;
  /**
   * Vertical background scroll position.
   */
  int8_t scroll_y;
  /**
   * Direction the player is currently moving.
   */
  Direction move_direction;
  /**
   * Animation step for the move.
   */
  uint8_t move_step;
  /**
   * Current VRAM progressive load origin x-position.
   */
  int8_t vram_x;
  /**
   * Curren VRAM progressive load origin y-position.
   */
  int8_t vram_y;
  /**
   * Tile buffer containing all map tiles to be progressively loaded while
   * scrolling the map during a move.
   */
  MapTile tile_buf[2 * MAP_HORIZ_LOADS];
  /**
   * Position in the buffer for the next map tile to be loaded.
   */
  uint8_t buffer_pos;
  /**
   * Maximum number of map tiles to load during a move.
   */
  uint8_t buffer_max;
  /**
   * VRAM start colum to use during a progressive load.
   */
  int8_t vram_col;
  /**
   * VRAM start row to use during a progressive load.
   */
  int8_t vram_row;
  /**
   * Amount added to the vram column position after loading each tile during a
   * progressive load.
   */
  int8_t vram_d_col;
  /**
   * Amount added to the vram row position after loading each tile during a
   * progressive load.
   */
  int8_t vram_d_row;
  /**
   * The active floor.
   */
  Floor *active_floor;
  /**
   * Active map in the active floor.
   */
  Map *active_map;
  /**
   * Map tile data for the tile the hero currently occupies and those in every
   * cardinal direction (index this with a `Direction`).
   */
  MapTile local_tiles[5];
  /**
   * State of the hero sprites.
   */
  HeroState hero_state;
  /**
   * Direction the hero is facing.
   */
  Direction hero_direction;
  /**
   * Hero walk animation timer.
   */
  Timer walk_timer;
  /**
   * Hero walk animation frame.
   */
  uint8_t walk_frame;
  /**
   * Reference to the exit prior to loading the destination for an exit.
   */
  Exit *current_exit;
} MapSystem;

/**
 * Main state for the map system.
 */
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
 * Sets position of map based on hero position. Does not re-render the map. You
 * must call `reset_map_screen()` to do this.
 */
inline void set_hero_position(int8_t x, int8_t y) {
  map.x = x - HERO_X_OFFSET;
  map.y = y - HERO_X_OFFSET;
}

/**
 * Sets the active floor for the map system. An active floor must be set prior
 * to initializing map system controller.
 * @param floor Floor to set.
 */
void set_active_floor(Floor *floor) BANKED;

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
 * Initiates battle using the currently configured encounter. This should be
 * called by area handlers to start battle after initalizing the `encounter`
 * state used by the battle system.
 */
void start_battle(void);

/**
 * Called when returning to the map system from the battle system.
 */
void return_from_battle(void) NONBANKED;

/**
 * Sets the active map.
 */
inline void set_active_map(const Map *m) NONBANKED {
  map.active_map = m;
}

/**
 * Opens a textbox while on the world map.
 * @param text Text to display in the text box.
 */
inline void map_textbox(const char *text) {
  map.state = MAP_STATE_TEXTBOX;
  textbox.open(text);
}

/**
 * @param c Column to check.
 * @param r Row to check.
 * @return `true` If the player is at the given column and row in the map.
 */
inline bool player_at(uint8_t c, uint8_t r) {
  return map.x + 4 == c && map.y + 4 == r;
}

/**
 * @param col Column to check.
 * @param row Row to check.
 * @param d Direction to check.
 * @return `true` If the player is at the given position in the map and is
 *   facing the given direction.
 */
inline bool player_at_facing(uint8_t col, uint8_t row, Direction d) {
  return map.x + 4 == col && map.y + 4 == row && map.hero_direction == d;
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
  if (map.active_floor->on_init)
    map.active_floor->on_init();
}

/**
 * Executes the active area's `on_update` callback if one is set.
 */
inline void on_update(void) {
  if (map.active_floor->on_update)
    map.active_floor->on_update();
}

/**
 * Executes the active area's `on_draw` callback if one is set.
 */
inline void on_draw(void) {
  if (map.active_floor->on_draw)
    map.active_floor->on_draw();
}

/**
 * Executes the active area's `on_action` callback if one is set.
 */
inline void on_action(void) {
  if (map.active_floor->on_action)
    map.active_floor->on_action();
}

/**
 * Executes the active area's `before_chest` callback if one is set.
 */
inline bool before_chest(Chest *chest) {
  if (map.active_floor->before_chest)
    return map.active_floor->before_chest(chest);
  return true;
}

/**
 * Executes the active area's `on_chest` callback if one is set.
 */
inline void on_chest(Chest *chest) {
  if (map.active_floor->on_chest)
    map.active_floor->on_chest(chest);
}

/**
 * Executes the active area's `on_enter` callback if one is set.
 */
inline void on_enter(uint8_t from_id, uint8_t to_id) {
  if (map.active_floor->on_enter)
    map.active_floor->on_enter(from_id, to_id);
}

/**
 * Executes the active area's `on_special` callback if one is set.
 */
inline bool on_special(void) {
  if (map.active_floor->on_special)
    return map.active_floor->on_special();
  return false;
}

/**
 * Executes the active area's `on_exit` callback if one is set.
 */
inline bool on_exit(void) {
  if (map.active_floor->on_exit)
    return map.active_floor->on_exit();
  return false;
}

/**
 * Executes the active area's `on_move` callback if one is set.
 */
inline bool on_move(void) {
  if (map.active_floor->on_move)
    return map.active_floor->on_move();
  return false;
}

#endif
