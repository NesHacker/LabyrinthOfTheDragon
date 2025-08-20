#ifndef _MAP_H
#define _MAP_H

#include <gb/gb.h>
#include <stdbool.h>
#include <stdint.h>

#include "core.h"
#include "item.h"
#include "monster.h"
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
 * Mask used to isolate a map data byte's "tile id".
 */
#define MAP_TILE_MASK 0b00111111

/**
 * Mask used to isolate a map data byte's "attribute".
 */
#define MAP_ATTR_MASK 0b11000000

/**
 * Tile id for the torch gauge "zero" sprite.
 */
#define TORCH_GAUGE_ZERO 0x20

/**
 * Tile id for the torch gauge cap.
 */
#define TORCH_GAUGE_CAP 0x29

/**
 * Sprite id for the torch gauge "flame"
 */
#define TORCH_GAUGE_FLAME 24

/**
 * Sprite id for the torch gauge body positon 1.
 */
#define TORCH_GAUGE_BODY_1 25

/**
 * Sprite id for the torch gauge body positon 2.
 */
#define TORCH_GAUGE_BODY_2 26

/**
 * Sprite id for the torch gauge body positon 3.
 */
#define TORCH_GAUGE_BODY_3 27

/**
 * Sprite id for the torch gauge body positon 4.
 */
#define TORCH_GAUGE_BODY_4 28

/**
 * Sprite property for all torch gauge sprites.
 */
#define TORCH_GAUGE_PROP 0b00001100

/**
 * X position for the torch gauge on the screen.
 */
#define TORCH_GAUGE_X 16

/**
 * Y position for the torch gauge on the screen
 */
#define TORCH_GAUGE_Y 24

/**
 * Flame tile animation frame 1.
 */
#define FLAME_TILE_1 0x04

/**
 * Flame tile animation frame 2.
 */
#define FLAME_TILE_2 0x14

/**
 * Base properties for all flame sprites.
 */
#define FLAME_SPRITE_PROP 0b00001000

/**
 * Id of the first designated flame sprite.
 */
#define FLAME_SPRITE_ID0 32

/**
 * Maximum number of flame sprites that can be on the screen at once.
 */
#define MAX_FLAME_SPRITES 8

/**
 * Clear tile for sprites.
 */
#define SPRITE_TILE_CLEAR 0x74

/**
 * Number of frames before reducing the torch gauge by 1.
 */
#define TORCH_GAUGE_SPEED 10

/**
 * Palette index for the torch gauge.
 */
#define TORCH_GAUGE_PALETTE 4

/**
 * Horizontal position of the magic keys HUD sprites.
 */
#define MAGIC_KEYS_X 16 + 8 * 6

/**
 * Vertical position for the magic keys HUD sprites.
 */
#define MAGIC_KEYS_Y 22

/**
 * Sprite id for the top "key" graphic sprite.
 */
#define MAGIC_KEY_SPRITE_1 29

/**
 * Sprite id for the bottom "key" graphic sprite.
 */
#define MAGIC_KEY_SPRITE_2 30

/**
 * Sprite id for the quantity of keys
 */
#define MAGIC_KEY_QTY 31

/**
 * Attribute to use for all magic key HUD sprites.
 */
#define MAGIC_KEY_HUD_ATTR 0b00001101

/**
 * Tile id for the top portion of the magic key.
 */
#define MAGIC_KEY_TOP_TILE 0x05

/**
 * Tile id for the bottom porition of the magic key.
 */
#define MAGIC_KEY_BOTTOM_TILE 0x15

/**
 * Tile id for the number 0 for the key quantity.
 */
#define MAGIC_KEY_NUM_0 0x30

/**
 * Palette to use for the magic keys hud.
 */
#define MAGIC_KEY_HUD_PALETTE 5

/**
 * Starting sprite id for NPC 1.
 */
#define NPC_SPRITE_1 4

/**
 * Starting sprite id for NPC 2.
 */
#define NPC_SPRITE_2 8

/**
 * Root tile number for the first sprite of NPC 1.
 */
#define NPC_1_TILE_ROOT 0x20

/**
 * Root tile number for the first sprite of NPC 2.
 */
#define NPC_2_TILE_ROOT 0x40

/**
 * Base property for NPC sprites.
 */
#define NPC_BASE_PROP 0x00

/**
 * Maximum maps per floor.
 */
#define MAX_MAPS 4 + 1

/**
 * Maximum exits per floor.
 */
#define MAX_EXITS 24 + 1

/**
 * Maximum chests per floor.
 */
#define MAX_CHESTS 8 + 1

/**
 * Maximum signs per floor.
 */
#define MAX_SIGNS 8 + 1

/**
 * Maximum levers per floor.
 */
#define MAX_LEVERS 8 + 1

/**
 * Max sconces per floor.
 */
#define MAX_SCONCES 32 + 1

/**
 * Max NPCs per floor.
 */
#define MAX_NPCS 2 + 1

/**
 * Max doors per floor.
 */
#define MAX_DOORS 12 + 1

/**
 * Sconce flame sprite ids.
 */
typedef enum SconceFlames {
  FLAME_1 = 32,
  FLAME_2 = 33,
  FLAME_3 = 34,
  FLAME_4 = 35,
  FLAME_5 = 36,
  FLAME_6 = 37,
  FLAME_7 = 38,
  FLAME_8 = 39,
} SconceFlames;

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
  /**
   * If the tile contains a door, this will point to it.
   */
  const struct Door *door;
  /**
   * If the tile contains a sign, this will point to it.
   */
  const struct Sign *sign;
  /**
   * If the tile contains a sconce, this will point to it.
   */
  const struct Sconce *sconce;
  /**
   * If the tile contains an NPC, this point to it.
   */
  const struct NPC *npc;
  /**
   * Whether or not the tile was marked as BG priority for rendering in the tile
   * source data.
   */
  bool bg_priority;
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
   * Denotes that the textbox should be opened with the preset text.
   */
  MAP_STATE_TEXTBOX_OPEN,
  /**
   * Denotes that the textbox is currently displayed.
   */
  MAP_STATE_TEXTBOX,
  /**
   * Denotes that battle was initiated from outside the map system and that
   * gameplay should be transitioned.
   */
  MAP_STATE_INITIATE_BATTLE,
  /**
   * Map is being transitioned to the battle system.
   */
  MAP_STATE_START_BATTLE,
  /**
   * Map is returning from battle.
   */
  MAP_STATE_FROM_BATTLE,
  /**
   * The map menu is being shown or animated.
   */
  MAP_STATE_MENU,
  /**
   * The map should teleport the player to the specified location.
   */
  MAP_STATE_TELEPORT,
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
   * Must be in the range of 0 - 127 to support various signed comparisons.
   */
  uint8_t width;
  /**
   * Height for the map (in map tiles).
   * Must be in the range of 0 - 127 to support various signed comparisons.
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
  ExitType exit_type;
  /**
   * Floor for the destination (optional).
   */
  struct FloorBank *to_floor;
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
   * Custom callback to execute on open. Allows for scripting chests.
   * @param chest The chest the player is attempting to open.
   * @return `true` if the default opening behavior for the chest should be
   *   prevented.
   */
  bool (*on_open)(const struct Chest *chest);
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
  DOOR_1 = FLAG16(0),
  DOOR_2 = FLAG16(1),
  DOOR_3 = FLAG16(2),
  DOOR_4 = FLAG16(3),
  DOOR_5 = FLAG16(4),
  DOOR_6 = FLAG16(5),
  DOOR_7 = FLAG16(6),
  DOOR_8 = FLAG16(7),
  DOOR_9 = FLAG16(8),
  DOOR_10 = FLAG16(9),
  DOOR_11 = FLAG16(10),
  DOOR_12 = FLAG16(11),
  DOOR_13 = FLAG16(12),
  DOOR_14 = FLAG16(13),
  DOOR_15 = FLAG16(14),
} DoorId;

/**
 * Type for the door.
 */
typedef enum DoorOpenGraphic {
  DOOR_NORMAL = 0x4E,
  DOOR_STAIRS_UP = 0x60,
  DOOR_STAIRS_DOWN = 0x88,
  DOOR_NEXT_LEVEL = 0x82,
} DoorOpenGraphic;

/**
 * Closed graphics for doors.
 */
typedef enum DoorClosedGraphic {
  DOOR_CLOSED_NORMAL = 0x64,
  DOOR_CLOSED_KEY = 0x62,
  DOOR_CLOSED_NEXT_LEVEL = 0x84,
} DoorClosedGraphic;

/**
 * A door that can be opened, closed, and locked.
 */
typedef struct Door {
  DoorId id;
  MapId map_id;
  int8_t col;
  int8_t row;
  DoorOpenGraphic type;
  bool magic_key_unlock;
  bool is_open;
} Door;

/**
 * Ids/Flags used to denote specific sconces.
 */
typedef enum SconceId {
  SCONCE_STATIC = 0,
  SCONCE_1 = FLAG(0),
  SCONCE_2 = FLAG(1),
  SCONCE_3 = FLAG(2),
  SCONCE_4 = FLAG(3),
  SCONCE_5 = FLAG(4),
  SCONCE_6 = FLAG(5),
  SCONCE_7 = FLAG(6),
  SCONCE_8 = FLAG(7),
} SconceId;

/**
 * Used to define the flame color for a sconce.
 */
typedef enum FlameColor {
  FLAME_NONE,
  FLAME_RED,
  FLAME_GREEN,
  FLAME_BLUE,
} FlameColor;

/**
 * Sprite property to use for red flames.
 */
#define FLAME_RED_PROP 0b00001001

/**
 * Sprite property to use for green flames.
 */
#define FLAME_GREEN_PROP 0b00001010

/**
 * Sprite property to use for blue flames.
 */
#define FLAME_BLUE_PROP 0b00001011

/**
 * A sconce that can be lit.
 */
typedef struct Sconce {
  /**
   * Id of the sconce.
   */
  SconceId id;
  /**
   * Map where the sconce resides.
   */
  MapId map_id;
  /**
   * Column for the sconce.
   */
  int8_t col;
  /**
   * Row for the sconce.
   */
  int8_t row;
  /**
   * Whether or not the sconce starts lit.
   */
  bool is_lit;
  /**
   * Color of the sconce's flame if it started lit.
   */
  FlameColor color;
  /**
   * Callback to execute when the sconce is lit.
   */
  void (*on_lit)(const struct Sconce *sconce);
} Sconce;

/**
 * Ids/Flags used to denote specific NPCs.
 */
typedef enum NpcId {
  NPC_1 = FLAG(0),
  NPC_2 = FLAG(1),
} NpcId;

/**
 * An NPC that can inhabit a map.
 */
typedef struct NPC {
  /**
   * Unique ID for the NPC.
   */
  NpcId id;
  /**
   * The map where the NPC should be placed.
   */
  MapId map_id;
  /**
   * Column in the map for the NPC.
   */
  int8_t col;
  /**
   * Row in the map for the NPC.
   */
  int8_t row;
  /**
   * The type of monster that the NPC is (determines the NPC graphic).
   */
  MonsterType monster_type;
  /**
   * Power tier for the monster (used to select palette).
   */
  PowerTier power_tier;
  /**
   * Scripting callback to execute when the player interacts with the NPC.
   * @param npc The NPC that initiated the callback.
   * @return `true` if the default action should be skipped.
   */
  bool (*on_action)(const struct NPC *npc);
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
   * Default starting column for the player on the starting map.
   */
  uint8_t default_x;
  /**
   * Default starting row for the player on the starting map.
   */
  uint8_t default_y;
  /**
   */
  palette_color_t *palettes;
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
   * Called when the map is initialized (happens on load, after battles, etc.).
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_init)(void);
  /**
   * Called when the map is active and the player moves into a special tile.
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_special)(void);
  /**
   * Called when the map is active and the player moves into a floor tile.
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_move)(void);
  /**
   * Called when the player presses the 'A' button on the map. Allows for custom
   * action scripting.
   * @return `true` if the map should prevent default behavior.
   */
  const bool (*on_action)(void);
  /**
   * Called when the floor is loaded.
   */
  const void (*on_load)(void);
  /**
   * Called on render frames to update graphics for the floor.
   */
  const void (*on_draw)(void);
} Floor;

/**
 * Data bank / floor data pair for the map system's floor loader.
 */
typedef struct FloorBank {
  /**
   * Bank the floor is on.
   */
  uint8_t bank;
  /**
   * Pointer to the floor's data.
   */
  Floor *floor;
} FloorBank;

/**
 * Number of entries in the map object hash table.
 * This must be a power of two for index wrapping logic.
 */
#define TILE_HASHTABLE_SIZE 64

/**
 * Denotes the type of data stored in a hash entry.
 */
typedef enum TileHashType {
  HASH_TYPE_CHEST,
  HASH_TYPE_LEVER,
  HASH_TYPE_DOOR,
  HASH_TYPE_SIGN,
  HASH_TYPE_SCONCE,
  HASH_TYPE_NPC,
} TileHashType;

/**
 * Entry for the tile objects hash table.
 */
typedef struct TileHashEntry {
  /**
   * Map id for the associated tile in the map.
   */
  uint8_t map_id;
  /**
   * Horizontal position for the associated tile in the map.
   */
  int8_t x;
  /**
   * Vertical positon for the associated tile in the map.
   */
  int8_t y;
  /**
   * Type of data being hashed.
   */
  TileHashType type;
  /**
   * Pointer to the data associated with the tile.
   */
  void *data;
} TileHashEntry;


typedef struct TileOverrideHashEntry {
  /**
   * Map id for the associated tile in the map.
   */
  uint8_t map_id;
  /**
   * Horizontal position for the associated tile in the map.
   */
  int8_t x;
  /**
   * Vertical positon for the associated tile in the map.
   */
  int8_t y;
  /**
   * Overrides the tile index.
   */
  uint8_t tile;
  /**
   * Override for the tile palette.
   */
  uint8_t palette;
} TileOverrideHashEntry;

/**
 * Enumeration of map menu states.
 */
typedef enum MapMenuState {
  MAP_MENU_CLOSED,
  MAP_MENU_OPEN,
} MapMenuState;

/**
 * Cursor positions for the map menu.
 */
typedef enum MapMenuCursor {
  MAP_MENU_CURSOR_SAVE,
  MAP_MENU_CURSOR_QUIT,
} MapMenuCursor;

/**
 * Map menu state data.
 */
typedef struct MapMenu {
  MapMenuState state;
  MapMenuCursor cursor;
} MapMenu;

/**
 * Map menu global state.
 */
extern MapMenu map_menu;

/**
 * Map tile data for the tile the hero currently occupies and those in every
 * cardinal direction (index this with a `Direction`).
 */
extern MapTile local_tiles[5];

/**
 * Whether or not the random seed should be initialized before the first move
 * on the map. Set this to false when using a specific seed in testing.
 */
extern bool init_random;

/**
 * Top-level state for the map system.
 */
extern MapState map_state;

/**
 * Horizontal map coordinate.
 */
extern int8_t map_x;

/**
 * Veritcal map coordinate.
 */
extern int8_t map_y;

/**
 * Horizontal background scroll position.
 */
extern int8_t map_scroll_x;

/**
 * Vertical background scroll position.
 */
extern int8_t map_scroll_y;

/**
 * Direction the hero is facing.
 */
extern Direction hero_direction;

/**
 * Chest "opened" flags for the current floor.
 */
extern uint8_t flags_chest_open;

/**
 * Chest "locked" flags for the current floor.
 */
extern uint8_t flags_chest_locked;

/**
 * Lever on/off states for the current floor.
 */
extern uint8_t flags_lever_on;

/**
 * Lever stuck/unstuck states for the current floor.
 */
extern uint8_t flags_lever_stuck;

/**
 * Door locked open/closed state.
 */
extern uint16_t flags_door_locked;

/**
 * Whether or not particular sconces are lit.
 */
extern uint8_t flags_sconce_lit;

/**
 * Whether or not NPCs are visible.
 */
extern uint8_t npc_visible;

/**
 * Message to set when opening the map textbox.
 */
extern const char *textbox_message;

/**
 * Each bit represents a "changed" flag for doors. This will be set if the
 * state of the door is changed via scripts, etc.
 */
extern uint16_t doors_updated;

/**
 * Each bit represents a "changed" flag for sconces. This will be set if the
 * stat of a sconce changes (if it was lit, etc.).
 */
extern uint8_t sconces_updated;

/**
 * Set if the player's HP and SP were updated as a result of a floor script.
 */
extern bool player_hp_and_sp_updated;

/**
 * Set to tell the system to reset local tiles on the next game loop update.
 */
extern bool refresh_local_tiles;

/**
 * Exit used for exit movement and teleporting.
 */
extern Exit active_exit;

/**
 * Flame colors for sconces that can be lit by the player.
 */
extern FlameColor sconce_colors[8];

/**
 * Debugging flag that allows the player to pass through locked doors.
 */
extern bool pass_doors;

typedef struct MapCallbacks {
  /**
   * Scripting callback to execute before closing the map textbox. This callback
   * is cleared every time the textbox is closed.
   * @return `true` to override default textbox closing behavior.
   */
  bool (*after_textbox)(void);
} MapCallbacks;

/**
 * Encapsulates various map callbacks used by helpers and external systems.
 */
extern MapCallbacks map_callbacks;

/**
 * Sets the position of the active map. Note this method will not re-render the
 * map based on the position set. You must call `refresh_map_screen()` or load
 * the screen progressively.
 * @param x Horizontal position.
 * @param y Vertical position.
 */
inline void set_map_position(int8_t x, int8_t y) {
  map_x = x;
  map_y = y;
}

/**
 * Sets position of map based on hero position. Does not re-render the map. You
 * must call `refresh_map_screen()` or load the screen progresively.
 */
inline void set_hero_position(int8_t x, int8_t y) {
  map_x = x - HERO_X_OFFSET;
  map_y = y - HERO_X_OFFSET;
}

/**
 * Sets the active floor for the map system. An active floor must be set prior
 * to initializing map system controller.
 * @param floor Floor to set.
 */
void set_active_floor(FloorBank *f) BANKED;

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
inline void start_battle(void) {
  map_state = MAP_STATE_INITIATE_BATTLE;
}

/**
 * Called when returning to the map system after death.
 */
void return_from_death(void) NONBANKED;

/**
 * Called when returning to the map system from the battle system.
 */
void return_from_battle(void) NONBANKED;

/**
 * Clears all active map sprites.
 */
void clear_map_sprites(void);

/**
 * Initializes the map menu.
 */
void init_map_menu(void);

/**
 * Performs game loop updates for the map menu.
 */
void update_map_menu(void);

/**
 * Updates HP/SP for the map menu.
 */
void update_map_menu_hp_sp(void);

/**
 * Opens the map menu.
 */
void show_map_menu(void);

/**
 * Closes the map menu.
 */
void hide_map_menu(void);

/**
 * Forces the player to take an exit.
 * @param exit Exit to force down the player's throat. Even if they don't want
 *   it.
 */
void take_exit(Exit *exit);

/**
 * Transports a player to the given map, column, and row.
 * @param map Id of the map.
 * @param col Column in the map.
 * @param row Row in the map.
 * @param heading Direction the player should "walk out" of the destination.
 * @param exit_type Type for the exit (primarily handles SFX).
 */
inline void teleport(
  MapId to_map,
  uint8_t col,
  uint8_t row,
  Direction heading,
  ExitType exit_type
) {
  active_exit.to_map = to_map;
  active_exit.to_col = col;
  active_exit.to_row = row;
  active_exit.to_floor = NULL;
  active_exit.heading = heading;
  active_exit.exit_type = exit_type;
  map_state = MAP_STATE_TELEPORT;
}

/**
 * Opens a textbox while on the world map.
 * @param text Text to display in the text box.
 */
inline void map_textbox(const char *text) {
  map_state = MAP_STATE_TEXTBOX_OPEN;
  textbox_message = text;
}

/**
 * Opens a textbox that executes the given action as it is closed.
 * @param text Text to display in the text box.
 * @param action Action to execute.
 */
inline void map_textbox_with_action(const char *text, bool (*action)(void)) {
  map_callbacks.after_textbox = action;
  map_textbox(text);
}

/**
 * @param c Column to check.
 * @param r Row to check.
 * @return `true` If the player is at the given column and row in the map.
 */
inline bool player_at(uint8_t c, uint8_t r) {
  return map_x + 4 == c && map_y + 4 == r;
}

/**
 * @param col Column to check.
 * @param row Row to check.
 * @param d Direction to check.
 * @return `true` If the player is at the given position in the map and is
 *   facing the given direction.
 */
inline bool player_at_facing(uint8_t col, uint8_t row, Direction d) {
  return map_x + 4 == col && map_y + 4 == row && hero_direction == d;
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
 * @return The x-position of the player in the current map.
 */
inline int8_t hero_x(void) {
  return map_x + HERO_X_OFFSET;
}

/**
 * @return The y-position of the player in the current map.
 */
inline int8_t hero_y(void) {
  return map_y + HERO_Y_OFFSET;
}

/**
 * Sets the chest as "opened" in the map system state.
 * @param chest Chest to set as opened.
 */
inline void set_chest_open(ChestId id) {
  flags_chest_open |= id;
}

/**
 * @return `true` if the chest with the given id is open.
 * @param id Id of the chest to check.
 */
inline bool is_chest_open(ChestId id) {
  return flags_chest_open & id;
}

/**
 * Sets the chest as "locked" in the map system state.
 * @param chest Chest to set as locked.
 */
inline void set_chest_locked(ChestId id) {
  flags_chest_locked |= id;
}

/**
 * Sets a chest as "unlocked" in the map system state.
 * @param chest Chest to set as unlocked.
 */
inline void set_chest_unlocked(ChestId id) {
  flags_chest_locked &= ~id;
}

/**
 * @return `true` if the chest is locked.
 * @param chest The chest to check.
 */
inline bool is_chest_locked(ChestId id) {
  return flags_chest_locked & id;
}

/**
 * @return `true` if the lever is on.
 * @param level The lever to test.
 */
inline bool is_lever_on(LeverId id) {
  return flags_lever_on & id;
}

/**
 * Changes the state of the lever to "on". This has no effect on the graphics
 * for the level.
 * @param lever The lever to toggle.
 */
inline bool toggle_lever_state(LeverId id) {
  flags_lever_on ^= id;
  *debug = flags_lever_on;
}

/**
 * @return `true` if the lever is stuck.
 * @param lever The lever to check.
 */
inline bool is_lever_stuck(LeverId id) {
  return flags_lever_stuck & id;
}

/**
 * Sets the lever "stuck" state to "on". Has no effect on graphics.
 * @param lever The lever to stick.
 */
inline void stick_lever(LeverId id) {
  flags_lever_stuck |= id;
}

/**
 * Sets the lever "stuck" state to "off". Has no effect on graphics.
 * @param lever The lever to unstick.
 */
inline void unstick_lever(LeverId id) {
  flags_lever_stuck &= ~id;
}

/**
 * @return If a door is open or not.
 * @param id Id of the door to test.
 */
inline bool is_door_open(DoorId id) {
  return ~flags_door_locked & id;
}

/**
 * Closes a door with the given id.
 * @param id Id of the door to close.
 */
inline void close_door(DoorId id) {
  if (!is_door_open(id))
    return;
  flags_door_locked |= id;
  doors_updated |= id;
}

/**
 * Opens the door with the given id.
 * @param id Id of the door to open.
 */
inline void open_door(DoorId id) {
  if (is_door_open(id))
    return;
  flags_door_locked &= ~id;
  doors_updated |= id;
}

/**
 * Toggles a door open and closed.
 * @param id Id of the door to toggle.
 */
inline void toggle_door(DoorId id) {
  if (is_door_open(id))
    close_door(id);
  else
    open_door(id);
}

/**
 * @return `true` if the sconce with the given id is lit.
 * @param id Id of the sconce to test.
 */
inline bool is_sconce_lit(SconceId id) {
  return id == SCONCE_STATIC || (flags_sconce_lit & id);
}

/**
 * @return The flame sprite id for the given sconce.
 * @param s The id of the sconce.
 */
uint8_t get_sconce_flame_sprite(SconceId sconce_id) NONBANKED;

/**
 * @return Index for the sconce with the given id.
 * @param id Id of the sconce.
 */
uint8_t get_sconce_index(SconceId sconce_id) NONBANKED;

/**
 * Lights a sconce.
 * @param id Id of the sconce to light.
 * @param color Color of the flame.
 */
inline void light_sconce(SconceId id, FlameColor color) {
  if (id == SCONCE_STATIC)
    return;
  flags_sconce_lit |= id;
  sconces_updated |= id;
  sconce_colors[get_sconce_index(id)] = color;
  const uint8_t sprite = get_sconce_flame_sprite(id);
  set_sprite_prop(sprite, FLAME_SPRITE_PROP | color);
}

/**
 * @param id Id of the sconce for which to get the color.
 * @return The color of the sconce if lit.
 */
inline FlameColor get_sconce_color(SconceId id) {
  return sconce_colors[get_sconce_index(id)];
}

/**
 * Extinguishes a sconce.
 * @param id Id of the sconce to extinguish.
 */
inline void extinguish_sconce(SconceId id) {
  flags_sconce_lit &= ~id;
}

/**
 * @return `true` if the npc with the given id is visible.
 * @param id Id of the NPC to check.
 */
inline bool is_npc_visible(NpcId id) {
  return npc_visible & id;
}

/**
 * Sets an NPC to be visible.
 * @param id Id of the NPC to set.
 */
inline void set_npc_visible(NpcId id) {
  npc_visible |= id;
  refresh_local_tiles = true;
}

/**
 * Sets and NPC to be invisible.
 * @param id Id of the NPC to set.
 */
inline void set_npc_invisible(NpcId id) {
  npc_visible &= ~id;
  refresh_local_tiles = true;
}

/**
 * Sets the palette for a given map coordinate.
 * @param map_id Id of the map.
 * @param x X coordinate for the tile on the map.
 * @param y Y coordinate for the tile on the map.
 * @param palette Palette to set.
 */
void set_palette_at(uint8_t map_id, int8_t x, int8_t y, uint8_t palette) BANKED;

/**
 * Overrides the graphics tile for a given map coordinate.
 * @param map_id Id of the map.
 * @param x X coordinate for the tile on the map.
 * @param y Y coordinate for the tile on the map.
 * @param tile Tile to set.
 */
void set_tile_at(uint8_t map_id, int8_t x, int8_t y, uint8_t tile) BANKED;

/**
 * Remaps an exit's destination.
 */
void remap_exit(
  uint8_t index,
  MapId to_map,
  uint8_t col,
  uint8_t row,
  Direction heading
);

#endif
