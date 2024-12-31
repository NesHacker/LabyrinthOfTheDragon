#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define FLOOR_TWO_ID 98
#define FLOOR_TWO_DEFAULT_X 6
#define FLOOR_TWO_DEFAULT_Y 12
#define FLOOR_TWO_DEFAULT_MAP MAP_A

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

const Map floor_two_maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_9, floor_two_data, 32, 22 },

  { END },
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

const uint16_t floor_two_palettes[] = {
  // Palette 1 - Core background tiles
  RGB8(190, 200, 190),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 2 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 3
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
  // Palette 4
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
  // Palette 5
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
  // Palette 6
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
  // Palette 7
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
};

//------------------------------------------------------------------------------
// Chests
//------------------------------------------------------------------------------

const Chest floor_two_chests[] = {
  /*
  {
    CHEST_1,    // Always use a CHEST_* here cause it acts like a flag
    MAP_A,      // Map where the chest resides
    0, 0,       // (x, y) location for the chest
    false,      // Whether or not the chest is locked
    false,      // If locked, can it be opened using a magic key?
    "Nice!",    // Message to display when the chest is opened
    NULL,       // Item list contents (optional)
    NULL,       // Scripting "on open" callback (optional)
  }
  */
  {
    CHEST_1,
    MAP_A, 20, 11, false, false,
    str_chest_item_2pot_1eth,
    chest_item_2pot_1eth,
  },
  {
    CHEST_2,
    MAP_A, 24, 10, false, false,
    str_chest_item_1pot,
    chest_item_1pot,
  },
  {
    CHEST_3,
    MAP_A, 18, 3, false, false,
    NULL,
    NULL,
    chest_add_magic_key,
  },

  { END },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

const Exit floor_two_exits[] = {
    /*
    {
      MAP_A,        // Map the exit is on
      0, 0,         // Column and row on that map for the exit
      FLOOR_TWO_ID,    // Floor to which the exit leads (last door, basically)
      DEST_MAP      // Id of the destination map
      0, 0,         // Column and row
      UP,           // Way the player should be facing leaving the exit
      EXIT_STAIRS   // Type of exit (not sure if we'll use this yet)
    },
    */

    {MAP_A, 2, 9, MAP_A, 21, 2, DOWN, EXIT_STAIRS},
    {MAP_A, 21, 2, MAP_A, 2, 9, DOWN, EXIT_STAIRS},

    {MAP_A, 10, 9, MAP_A, 14, 2, DOWN, EXIT_STAIRS},
    {MAP_A, 14, 2, MAP_A, 10, 9, DOWN, EXIT_STAIRS},

    {MAP_A, 29, 2, MAP_A, 4, 19, DOWN, EXIT_STAIRS},
    {MAP_A, 4, 19, MAP_A, 29, 2, DOWN, EXIT_STAIRS},

    {MAP_A, 12, 17, MAP_A, 18, 16, DOWN, EXIT_STAIRS},
    {MAP_A, 18, 16, MAP_A, 12, 17, DOWN, EXIT_STAIRS},

    {MAP_A, 6, 8, MAP_A, 9, 2, DOWN, EXIT_STAIRS},
    {MAP_A, 9, 2, MAP_A, 6, 8, DOWN, EXIT_STAIRS},

    {MAP_A, 3, 1, MAP_A, 6, 12, UP, EXIT_STAIRS, &floor_two},

    {END},
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

const Sign floor_two_signs[] = {
  /*
  {
    MAP_A,      // Id of the map
    0, 0,       // Position in the map for the sign
    UP,         // Direction the player must be facing
    "Hi there!" // The message to display
  }
  */
  { MAP_A,  5, 8, UP, "Lite Fires,\n to open this\n Doors!" },
  { MAP_A, 26, 3, UP, "Something used\n have been here.\n" },

  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

const void floor_two_on_lever(const Lever *lever) {
  if (lever->id == LEVER_1) {
    map_textbox("You hear a\nka-thunk\x60");
    open_door_by_id(DOOR_3);
  }
}

const Lever floor_two_levers[] = {
  /*
  {
    LEVER_1,  // Use the LEVER_* constants for ids (again, used as flags)
    MAP_A,    // The map where the lever be
    0, 0,     // (x, y) tile coordinates in the map
    false,    // Can the lever only be pulled once?
    false,    // Does the lever start stuck? (requires scripting to change)
    NULL,     // Scripting callback for the lever
  }
  */
  { LEVER_1, MAP_A, 3, 3, true, false, floor_two_on_lever },

  { END },
};

//------------------------------------------------------------------------------
// Doors
//------------------------------------------------------------------------------

const Door floor_two_doors[] = {
  /*
  {
    DOOR_1,           // Use DOOR_* constants for ids.
    MAP_A,            // Map for the door
    0, 0              // (x, y) tile for the door
    DOOR_STAIRS_UP    // Kind of door
    true              // Magic key required to unlock
  }
  */
  { DOOR_1, MAP_A,  6,  8, DOOR_STAIRS_DOWN, false },
  { DOOR_2, MAP_A, 20, 16, DOOR_NORMAL, true },
  { DOOR_3, MAP_A,  3,  1, DOOR_NEXT_LEVEL, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

void floor_two_on_lit(const Sconce* sconce) {
  if(is_sconce_lit(SCONCE_2))
    light_sconce(SCONCE_6, FLAME_RED);

  if(is_sconce_lit(SCONCE_3))
    light_sconce(SCONCE_7, FLAME_GREEN);

  if(is_sconce_lit(SCONCE_4))
    light_sconce(SCONCE_8, FLAME_BLUE);

  if(is_sconce_lit(SCONCE_5))
    light_sconce(SCONCE_1, FLAME_RED);

  if(
    is_sconce_lit(SCONCE_5) &&
    is_sconce_lit(SCONCE_6) &&
    is_sconce_lit(SCONCE_7) &&
    is_sconce_lit(SCONCE_8)
    ) {
    //open the door dave
    open_door_by_id(DOOR_1);
  }
}

const Sconce floor_two_sconces[] = {
  /*
  {
    SCONCE_1,   // Use SCONCE_* constants for ids.
    MAP_A,      // Map for the sconce
    0, 0,       // (x, y) tile for the sconce
    true,       // Should the sconce start lit
    FLAME_BLUE  // Flame color for the sconce if it starts lit.
  }
  */
  { SCONCE_STATIC, MAP_A, 4, 14, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 13, 12, true, FLAME_RED },

  { SCONCE_2, MAP_A,  8, 16, false, FLAME_RED, floor_two_on_lit },
  { SCONCE_3, MAP_A, 23, 16, false, FLAME_RED, floor_two_on_lit },
  { SCONCE_4, MAP_A, 15,  2, false, FLAME_RED, floor_two_on_lit },
  { SCONCE_5, MAP_A, 20,  2, false, FLAME_RED, floor_two_on_lit },

  { SCONCE_6, MAP_A, 4,  7, false, FLAME_RED },
  { SCONCE_7, MAP_A, 5,  7, false, FLAME_RED },
  { SCONCE_8, MAP_A, 7,  7, false, FLAME_RED },
  { SCONCE_1, MAP_A, 8,  7, false, FLAME_RED },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (NOT YET IMPLS)
//------------------------------------------------------------------------------

const NPC floor_two_npcs[] = {
  /*
  {
    NPC_1,    // Use NPC_* constants for ids.
    MAP_A,    // Map for the npc
    0, 0      // (x, y) tile for the npc
  }
  */
  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

bool floor_two_on_init(void) {
  return false;
}

void floor_two_on_update(void) {
}

void floor_two_on_draw(void) {
}

bool floor_two_on_action(void) {
  return false;
}

bool floor_two_on_special(void) {
  return false;
}

bool floor_two_on_exit(void) {
  return false;
}

bool floor_two_on_move(void) {
  return false;
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_two = {
  FLOOR_TWO_ID,                              // Id
  FLOOR_TWO_DEFAULT_MAP,                     // Default Map
  FLOOR_TWO_DEFAULT_X, FLOOR_TWO_DEFAULT_Y, // Default Starting (x, y)
  floor_two_palettes,                        // Palettes
  floor_two_maps,
  floor_two_exits,
  floor_two_chests,
  floor_two_signs,
  floor_two_levers,
  floor_two_doors,
  floor_two_sconces,
  floor_two_npcs,
  floor_two_on_init,
  floor_two_on_update,
  floor_two_on_draw,
  floor_two_on_action,
  floor_two_on_special,
  floor_two_on_exit,
  floor_two_on_move,
};

