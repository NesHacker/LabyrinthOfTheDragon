#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define FLOOR_ONE_ID 99
#define FLOOR_ONE_DEFAULT_X 7
#define FLOOR_ONE_DEFAULT_Y 31
#define FLOOR_ONE_DEFAULT_MAP MAP_A

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

#define FLOOR_ONE_NUM_MAPS 1

const Map floor_1_maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_9, floor_1_data, 32, 32 },
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

const uint16_t floor_1_palettes[] = {
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

#define FLOOR_ONE_NUM_CHESTS 3

typedef enum FLOOR_ONE_Chests {
  FLOOR_ONE_CHEST_TORCH,
  FLOOR_ONE_CHEST_SECRET,
  FLOOR_ONE_CHEST_KEY,
} FLOOR_ONE_Chests;

const Chest floor_1_chests[] = {
  /*
  {
    FLOOR_ONE_CHEST_0,   // id (use the enum above to define these)
    MAP_A,                // Map where the chest is at
    0, 0,                 // Column & row in that map
    FLAGS_CHEST_OPEN,     // Global flag page for the open/closed flag.
    CHEST_FLAG_1          // Open flag (the bit determines if it's been opened)
  }
  */
  { FLOOR_ONE_CHEST_TORCH,  MAP_A, 21, 18, FLAGS_CHEST_OPEN, CHEST_FLAG_1},
  { FLOOR_ONE_CHEST_KEY,    MAP_A, 23, 26, FLAGS_CHEST_OPEN, CHEST_FLAG_1},
  { FLOOR_ONE_CHEST_SECRET, MAP_A, 22,  9, FLAGS_CHEST_OPEN, CHEST_FLAG_1},
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

#define FLOOR_ONE_NUM_EXITS 4

const Exit floor_1_exits[] = {
  /*
  {
    MAP_A,        // Map the exit is on
    0, 0,         // Column and row on that map for the exit
    FLOOR_ONE_ID,    // Floor to which the exit leads (last door, basically)
    DEST_MAP      // Id of the destination map
    0, 0,         // Column and row
    UP,           // Way the player should be facing leaving the exit
    EXIT_STAIRS   // Type of exit (not sure if we'll use this yet)
  },
  */

 { MAP_A, 3, 25, MAP_A, 19, 17, DOWN, EXIT_STAIRS },
 { MAP_A, 19, 17, MAP_A, 3, 25, DOWN, EXIT_STAIRS },

 { MAP_A, 12, 25, MAP_A, 28, 18, DOWN, EXIT_STAIRS },
 { MAP_A, 28, 18, MAP_A, 12, 25, DOWN, EXIT_STAIRS },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

#define FLOOR_ONE_NUM_SIGNS 2

const Sign floor_1_signs[] = {
  /*
  {
    MAP_A,      // Id of the map
    0, 0,       // Position in the map for the sign
    UP,         // Direction the player must be facing
    "Hi there!" // The message to display
  }
  */
 { MAP_A, 4, 1, UP, "This skull\x60\nIs so metal." },
 { MAP_A, 7, 1, UP, "A pair of glowing\neyes peers back\x60" }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

void floor_1_on_init(void) {
}

void floor_1_on_update(void) {
}

void floor_1_on_draw(void) {
}

bool floor_1_on_action(void) {
  return false;
}

bool floor_1_before_chest(Chest *chest) {
  return false;
}

void floor_1_on_enter(uint8_t from_id, uint8_t to_id) {
}

void floor_1_on_chest(Chest *chest) {
  switch (chest->id) {
  default:
  case FLOOR_ONE_CHEST_KEY:
    // get the key for the boss door
    break;
  case FLOOR_ONE_CHEST_SECRET:
    // get a potion for the healing
    break;
  case FLOOR_ONE_CHEST_TORCH:
    // get the torch for the greater good
    break;
  }
}

bool floor_1_on_special(void) {
  switch (map.active_map->id) {
  case MAP_A:
    if (player_at(6, 7)) {
      MonsterInstance *monster = encounter.monsters;
      reset_encounter(MONSTER_LAYOUT_3S);
      dummy_generator(monster, player.level, DUMMY_COWARD);
      monster->id = 'A';
      dummy_generator(++monster, player.level, DUMMY_COWARD);
      monster->id = 'B';
      dummy_generator(++monster, player.level, DUMMY_COWARD);
      monster->id = 'C';
      start_battle();
      return true;
    }
    break;
  }
  return false;
}

bool floor_1_on_exit(void) {
  return false;
}

bool floor_1_on_move(void) {
  return false;
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_1 = {
  FLOOR_ONE_ID,                              // Id
  FLOOR_ONE_DEFAULT_MAP,                     // Default Map
  FLOOR_ONE_DEFAULT_X, FLOOR_ONE_DEFAULT_Y, // Default Starting (x, y)
  floor_1_palettes,                        // Palettes
  FLOOR_ONE_NUM_MAPS, floor_1_maps,       // # of maps, maps
  FLOOR_ONE_NUM_EXITS, floor_1_exits,     // # of exits, exits
  FLOOR_ONE_NUM_CHESTS, floor_1_chests,   // # of chests, chests
  FLOOR_ONE_NUM_SIGNS, floor_1_signs,     // # signs, signs
  floor_1_on_init,
  floor_1_on_update,
  floor_1_on_draw,
  floor_1_on_action,
  floor_1_before_chest,
  floor_1_on_chest,
  floor_1_on_enter,
  floor_1_on_special,
  floor_1_on_exit,
  floor_1_on_move,
};

