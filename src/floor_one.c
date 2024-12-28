#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define FLOOR_TEST_ID 99
#define FLOOR_TEST_DEFAULT_X 7
#define FLOOR_TEST_DEFAULT_Y 30
#define FLOOR_TEST_DEFAULT_MAP MAP_A

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

const Map floor_one_maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_9, floor_one_data, 32, 32 },
  { END },
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

const uint16_t floor_one_palettes[] = {
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

const Chest floor_one_chests[] = {
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
  // Basic items chest, contains some stuffs
  {
    CHEST_1,
    MAP_A, 21, 18, false, false,
    NULL,
    NULL,
    chest_add_torch,
  },
  // A locked chest that can be unlocked using scripting
  {
    CHEST_2,
    MAP_A, 23, 26, false, false,
    NULL,
    NULL,
    chest_add_magic_key,
  },
  // An empty chest, default behavior if you don't set any params
  {
    CHEST_3,
    MAP_A, 22, 9, false, false,
    str_chest_item_1pot,
    chest_item_1pot,
  },

  { END },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

const Exit floor_one_exits[] = {
    /*
    {
      MAP_A,        // Map the exit is on
      0, 0,         // Column and row on that map for the exit
      FLOOR_TEST_ID,    // Floor to which the exit leads (last door, basically)
      DEST_MAP      // Id of the destination map
      0, 0,         // Column and row
      UP,           // Way the player should be facing leaving the exit
      EXIT_STAIRS   // Type of exit (not sure if we'll use this yet)
    },
    */

    {MAP_A, 3, 25, MAP_A, 19, 17, DOWN, EXIT_STAIRS},
    {MAP_A, 19, 17, MAP_A, 3, 25, DOWN, EXIT_STAIRS},

    {MAP_A, 12, 25, MAP_A, 28, 18, DOWN, EXIT_STAIRS},
    {MAP_A, 28, 18, MAP_A, 12, 25, DOWN, EXIT_STAIRS},

    {MAP_A, 6, 18, MAP_A, 6, 13, UP, EXIT_STAIRS},
    {MAP_A, 6, 13, MAP_A, 6, 18, DOWN, EXIT_STAIRS},

    {MAP_A, 6, 3, MAP_A, 7, 31, UP, EXIT_STAIRS, &floor_one},

    {END},
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

const Sign floor_one_signs[] = {
  /*
  {
    MAP_A,      // Id of the map
    0, 0,       // Position in the map for the sign
    UP,         // Direction the player must be facing
    "Hi there!" // The message to display
  }
  */
  { MAP_A, 4, 25, UP, str_maps_sign_monster_no_fire },
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

void floor_one_on_lever(const Lever *lever) {
  if (lever->id == LEVER_1) {
    map_textbox("You hear a\nka-thunk\x60");
    open_door_by_id(DOOR_3);
  }
}

const Lever floor_one_levers[] = {
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
  { LEVER_1, MAP_A, 6, 7, true, false, floor_one_on_lever },

  { END },
};

//------------------------------------------------------------------------------
// Doors
//------------------------------------------------------------------------------

const Door floor_one_doors[] = {
  /*
  {
    DOOR_1,           // Use DOOR_* constants for ids.
    MAP_A,            // Map for the door
    0, 0              // (x, y) tile for the door
    DOOR_STAIRS_UP    // Kind of door
    true              // Magic key required to unlock
  }
  */
  { DOOR_1, MAP_A, 3, 25, DOOR_STAIRS_DOWN, true },
  { DOOR_2, MAP_A, 6, 18, DOOR_NORMAL, false },
  { DOOR_3, MAP_A, 6,  3, DOOR_NEXT_LEVEL, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

void floor_one_on_lit(const Sconce* sconce) {
  if(is_sconce_lit(SCONCE_1) && is_sconce_lit(SCONCE_2)) {
    //open the door dave
    open_door_by_id(DOOR_2);
  }
}

const Sconce floor_one_sconces[] = {
  /*
  {
    SCONCE_1,   // Use SCONCE_* constants for ids.
    MAP_A,      // Map for the sconce
    0, 0,       // (x, y) tile for the sconce
    true,       // Should the sconce start lit
    FLAME_BLUE  // Flame color for the sconce if it starts lit.
  }
  */
  { SCONCE_1, MAP_A, 4, 18, false, FLAME_RED, floor_one_on_lit },
  { SCONCE_2, MAP_A, 8, 18, false, FLAME_RED, floor_one_on_lit },

  // These do nothing Jon Snow
  { SCONCE_3, MAP_A,  5, 25, true, FLAME_RED },
  { SCONCE_4, MAP_A, 20, 17, true, FLAME_RED },
  { SCONCE_5, MAP_A, 22, 17, true, FLAME_RED },
  { SCONCE_6, MAP_A, 22,  6, true, FLAME_RED },


  { END }
};

//------------------------------------------------------------------------------
// NPCs (NOT YET IMPLS)
//------------------------------------------------------------------------------

const NPC floor_one_npcs[] = {
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

void floor_one_on_init(void) {
}

void floor_one_on_update(void) {
}

void floor_one_on_draw(void) {
}

bool floor_one_on_action(void) {
  return false;
}

bool floor_one_on_special(void) {
  // 21, 19
  if (player_at(21, 19)) {
    Monster *monster = encounter.monsters;
    reset_encounter(MONSTER_LAYOUT_1);
    kobold_generator(monster, player.level, C_TIER);
    monster->id = 'A';
    start_battle();
    return true;
  }

  return false;
}

bool floor_one_on_exit(void) {
  return false;
}

uint8_t enc_step = 0;
uint8_t enc_chance = 1;

bool floor_one_on_move(void) {
  enc_step++;
  if (player.torch_gauge > 0)
    return false;

  if (enc_step < 4)
    return false;

  if (d64() >= enc_chance) {
    enc_chance++;
    return false;
  }

  enc_step = 0;
  enc_chance = 1;

  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  beholder_generator(monster, player.level, C_TIER);
  monster->id = 'A';
  start_battle();
  return true;
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_one= {
  FLOOR_TEST_ID,                              // Id
  FLOOR_TEST_DEFAULT_MAP,                     // Default Map
  FLOOR_TEST_DEFAULT_X, FLOOR_TEST_DEFAULT_Y, // Default Starting (x, y)
  floor_one_palettes,                        // Palettes
  floor_one_maps,
  floor_one_exits,
  floor_one_chests,
  floor_one_signs,
  floor_one_levers,
  floor_one_doors,
  floor_one_sconces,
  floor_one_npcs,
  floor_one_on_init,
  floor_one_on_update,
  floor_one_on_draw,
  floor_one_on_action,
  floor_one_on_special,
  floor_one_on_exit,
  floor_one_on_move,
};

