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
  MAP_J,
  MAP_K,
  MAP_INVALID = 0xFF
} MapId;

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
  /**
   * If the tile contains a chest, this will point to it.
   */
  const struct Chest *chest;
  /**
   * If the tile contains a lever, this will point to it.
   */
  const struct Lever *lever;
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
   * Denotes that map system is currently loading the destination for an exit.
   */
  MAP_STATE_LOAD_EXIT,
  /**
   * Ending state for a exit load.
   */
  MAP_STATE_EXIT_LOADED,
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
  /**
   * Map is returning from battle.
   */
  MAP_STATE_FROM_BATTLE,
} MapState;

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
  /**
   * Floor for the destination (optional).
   */
  struct Floor *to_floor;
} Exit;

/**
 * Easy way to add a informative "sign" to open the text box and send the user
 * a message. Can be used on any tile.
 */
typedef struct Sign {
  /**
   * Map id where the sign resides.
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
   * Direction the player must be facing to read the sign.
   */
  Direction facing;
  /**
   * Message to display for the sign.
   */
  const char *message;
} Sign;

/**
 * Ids/Flags used to denote specific chests.
 */
typedef enum ChestId {
  CHEST_1 = FLAG(0),
  CHEST_2 = FLAG(1),
  CHEST_3 = FLAG(2),
  CHEST_4 = FLAG(3),
  CHEST_5 = FLAG(4),
  CHEST_6 = FLAG(5),
  CHEST_7 = FLAG(6),
  CHEST_8 = FLAG(7),
} ChestId;

/**
 * Data representation of a treasure chest in an area. Chests can be placed at
 * any position in any map. Whether or not they have been opened is determined
 * by a global flag (so the state can be saved to SRAM).
 */
typedef struct Chest {
  /**
   * Id for the chest. This should be the chest list array index.
   */
  ChestId id;
  /**
   * Id for the map where the chest resides.
   */
  uint8_t map_id;
  /**
   * Column for the chest.
   */
  int8_t col;
  /**
   * Row for the chest.
   */
  int8_t row;
  /**
   * Whether or not the chest starts as locked. (optional, default: false).
   */
  bool locked;
  /**
   * Can the chest be unlocked with a magic key? (optional, default: false).
   */
  bool magic_key_unlock;
  /**
   * Message to display when the chest is successfully opened (optional).
   */
  const char *open_msg;
  /**
   * Items the chest contains (optional).
   */
  const Item *items;
  /**
   * Custom callback to execute on open (for special scrited chests).
   * @param chest The chest the player is attempting to open.
   * @return `true` if the chest was opened.
   */
  const bool (*on_open)(const struct Chest *chest);
} Chest;

/**
 * Ids/Flags used to denote specific levers.
 */
typedef enum LeverId {
  LEVER_1 = FLAG(0),
  LEVER_2 = FLAG(1),
  LEVER_3 = FLAG(2),
  LEVER_4 = FLAG(3),
  LEVER_5 = FLAG(4),
  LEVER_6 = FLAG(5),
  LEVER_7 = FLAG(6),
  LEVER_8 = FLAG(7),
} LeverId;

/**
 * A lever that can be switched on/off.
 */
typedef struct Lever {
  /**
   * Id of the lever. Use `LEVER_*` constants as this will act as a lever's
   * on/off state flag.
   */
  LeverId id;
  /**
   * Map id where the lever resides.
   */
  MapId map_id;
  /**
   * Column for the lever.
   */
  int8_t col;
  /**
   * Row for the lever.
   */
  int8_t row;
  /**
   * Set this to true if the lever can only be pulled once.
   */
  bool one_shot;
  /**
   * Set this to true if the level should begin as "stuck". Requires scripting
   * to modify if this is set.
   */
  bool stuck;
  /**
   * Custom callback to execute when the lever is pulled.
   * @param lever The lever being pulled.
   */
  const void (*on_pull)(const struct Lever *lever);
} Lever;

/**
 * Ids/Flags used to denote specific doors.
 */
typedef enum DoorId {
  DOOR_1 = FLAG(0),
  DOOR_2 = FLAG(1),
  DOOR_3 = FLAG(2),
  DOOR_4 = FLAG(3),
  DOOR_5 = FLAG(4),
  DOOR_6 = FLAG(5),
  DOOR_7 = FLAG(6),
  DOOR_8 = FLAG(7),
} DoorId;

/**
 * A door that can be opened, closed, and locked. WARNING: NOT IMPLEMENTED YET.
 */
typedef struct Door {
  DoorId id;
  MapId map_id;
  int8_t col;
  int8_t row;
} Door;

/**
 * Ids/Flags used to denote specific sconces.
 */
typedef enum SconceId {
  SCONCE_1 = FLAG(0),
  SCONCE_2 = FLAG(1),
  SCONCE_3 = FLAG(2),
  SCONCE_4 = FLAG(3),
  SCONCE_5 = FLAG(4),
  SCONCE_6 = FLAG(5),
  SCONCE_7 = FLAG(6),
  SCONCE_8 = FLAG(7),
  /**
   * Denotes a sconce that starts lit.
   */
  SCONCE_LIT = 0xFF,
} SconceId;

/**
 * A sconce that can be lit. WARNING: Not yet implemented.
 */
typedef struct Sconce {
  SconceId id;
  MapId map_id;
  int8_t col;
  int8_t row;
} Sconce;

/**
 * Ids/Flags used to denote specific NPCs.
 */
typedef enum NpcId {
  NPC_1 = FLAG(0),
  NPC_2 = FLAG(1),
  NPC_3 = FLAG(2),
  NPC_4 = FLAG(3),
  NPC_5 = FLAG(4),
  NPC_6 = FLAG(5),
  NPC_7 = FLAG(6),
  NPC_8 = FLAG(7),
} NpcId;

/**
 * An NPC that can inhabit a map. WARNING: not yet implemented.
 */
typedef struct NPC {
  NpcId id;
  MapId map_id;
  int8_t col;
  int8_t row;
} NPC;

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
   * List of all maps for the floor.
   */
  const Map *maps;
  /**
   * List of exits for all pages on the floor.
   */
  const Exit *exits;
  /**
   * Array of treasure chests for the floor.
   */
  const Chest *chests;
  /**
   * Signs for the floor.
   */
  const Sign *signs;
  /**
   * Levers on the floor.
   */
  const Lever *levers;
  /**
   * Doors on a floor.
   */
  const Door *doors;
  /**
   * Sconces on a floor.
   */
  const Sconce *sconces;
  /**
   * NPCs on a floor.
   */
  const NPC *npcs;
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
   * Called when the player interacts by pressing the "A" button.
   * @return `true` to prevent the default action behavior.
   */
  const bool (*on_action)(void);
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
  const Floor *active_floor;
  /**
   * Active map in the active floor.
   */
  const Map *active_map;
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
   * Map state to set after a fade in/out is complete.
   */
  MapState fade_to_state;
  /**
   * Reference to the exit prior to loading the destination for an exit.
   */
  const Exit *active_exit;
  /**
   * Chest "opened" flags for the current floor.
   */
  uint8_t flags_chest_open;
  /**
   * Chest "locked" flags for the current floor.
   */
  uint8_t flags_chest_locked;
  /**
   * Lever on/off states for the current floor.
   */
  uint8_t flags_lever_on;
  /**
   * Lever stuck/unstuck states for the current floor.
   */
  uint8_t flags_lever_stuck;
} MapSystem;

/**
 * Main state for the map system.
 */
extern MapSystem map;

/**
 * Sets the position of the active map. Note this method will not re-render the
 * map based on the position set. You must call `refresh_map_screen()` or load
 * the screen progressively.
 * @param x Horizontal position.
 * @param y Vertical position.
 */
inline void set_map_position(int8_t x, int8_t y) {
  map.x = x;
  map.y = y;
}

/**
 * Sets position of map based on hero position. Does not re-render the map. You
 * must call `refresh_map_screen()` or load the screen progresively.
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
void update_world_map(void);

/**
 * VBLANK draw routine for the world map controller.
 */
void draw_world_map(void);

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
inline bool on_action(void) {
  if (map.active_floor->on_action)
    return map.active_floor->on_action();
  return false;
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

/**
 * @return The x-position of the player in the current map.
 */
inline int8_t hero_x(void) {
  return map.x + HERO_X_OFFSET;
}

/**
 * @return The y-position of the player in the current map.
 */
inline int8_t hero_y(void) {
  return map.y + HERO_Y_OFFSET;
}

/**
 * Sets the chest as "opened" in the map system state.
 * @param chest Chest to set as opened.
 */
inline void set_chest_open(ChestId id) {
  map.flags_chest_open |= id;
}

/**
 * Sets the chest as "locked" in the map system state.
 * @param chest Chest to set as locked.
 */
inline void set_chest_locked(ChestId id) {
  map.flags_chest_locked |= id;
}

/**
 * Sets a chest as "unlocked" in the map system state.
 * @param chest Chest to set as unlocked.
 */
inline void set_chest_unlocked(ChestId id) {
  map.flags_chest_locked &= ~id;
}

/**
 * @return `true` if the chest is locked.
 * @param chest The chest to check.
 */
inline bool is_chest_locked(ChestId id) {
  return map.flags_chest_locked & id;
}

/**
 * @return `true` if the lever is on.
 * @param level The lever to test.
 */
inline bool is_lever_on(const Lever *lever) {
  return map.flags_lever_on & lever->id;
}

/**
 * Changes the state of the lever to "on". This has no effect on the graphics
 * for the level.
 * @param lever The lever to toggle.
 */
inline bool toggle_lever_state(LeverId id) {
  map.flags_lever_on ^= id;
}

/**
 * @return `true` if the lever is stuck.
 * @param lever The lever to check.
 */
inline bool is_lever_stuck(LeverId id) {
  return map.flags_lever_stuck & id;
}

/**
 * Sets the lever "stuck" state to "on". Has no effect on graphics.
 * @param lever The lever to stick.
 */
inline void stick_lever(LeverId id) {
  map.flags_lever_stuck |= id;
}

/**
 * Sets the lever "stuck" state to "off". Has no effect on graphics.
 * @param lever The lever to unstick.
 */
inline void unstick_lever(LeverId id) {
  map.flags_lever_stuck &= ~id;
}


#endif
