#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define FLOOR_TEST_ID 99
#define FLOOR_TEST_DEFAULT_X 4
#define FLOOR_TEST_DEFAULT_Y 4
#define FLOOR_TEST_DEFAULT_MAP MAP_A

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

#define FLOOR_TEST_NUM_MAPS 3

const Map floor_test_maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_9, floor_test_data, 32, 32 },
  { MAP_B, BANK_9, floor_test_mini, 8, 8 },
  { MAP_C, BANK_9, floor_test_17x12, 17, 12 },
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

const uint16_t floor_test_palettes[] = {
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

#define FLOOR_TEST_NUM_CHESTS 0

typedef enum FLOOR_TEST_Chests {
  FLOOR_TEST_CHEST_0,
} FLOOR_TEST_Chests;

const Chest floor_test_chests[] = {
  /*
  {
    FLOOR_TEST_CHEST_0,   // id (use the enum above to define these)
    MAP_A,                // Map where the chest is at
    0, 0,                 // Column & row in that map
    FLAGS_CHEST_OPEN,     // Global flag page for the open/closed flag.
    CHEST_FLAG_1          // Open flag (the bit determines if it's been opened)
  }
  */
  { 0 }, // Remove me when you add your first chest (pointers amirite?)
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

#define FLOOR_TEST_NUM_EXITS 6

const Exit floor_test_exits[] = {
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

 { MAP_A, 8, 1, MAP_B, 4, 1, DOWN, EXIT_STAIRS },
 { MAP_B, 4, 1, MAP_A, 8, 1, DOWN, EXIT_STAIRS },

 { MAP_A, 25, 27, MAP_B, 4, 5, DOWN, EXIT_STAIRS },
 { MAP_B, 4, 5, MAP_A, 25, 27, DOWN, EXIT_STAIRS },

 { MAP_A, 9, 27, MAP_C, 3, 3, DOWN, EXIT_STAIRS },
 { MAP_C, 3, 3, MAP_A, 9, 27, DOWN, EXIT_STAIRS },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

#define FLOOR_TEST_NUM_SIGNS 2

const Sign floor_test_signs[] = {
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

void floor_test_on_init(void) {
}

void floor_test_on_update(void) {
}

void floor_test_on_draw(void) {
}

bool floor_test_on_action(void) {
  return false;
}

bool floor_test_before_chest(Chest *chest) {
  return false;
}

void floor_test_on_enter(uint8_t from_id, uint8_t to_id) {
}

void floor_test_on_chest(Chest *chest) {
  switch (chest->id) {
  default:
  case FLOOR_TEST_CHEST_0:
    // ...
    break;
  }
}

bool floor_test_on_special(void) {
  switch (map.active_map->id) {
  case MAP_A:
    if (player_at(3, 8)) {
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

bool floor_test_on_exit(void) {
  return false;
}

bool floor_test_on_move(void) {
  return false;
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_test = {
  FLOOR_TEST_ID,                              // Id
  FLOOR_TEST_DEFAULT_MAP,                     // Default Map
  FLOOR_TEST_DEFAULT_X, FLOOR_TEST_DEFAULT_Y, // Default Starting (x, y)
  floor_test_palettes,                        // Palettes
  FLOOR_TEST_NUM_MAPS, floor_test_maps,       // # of maps, maps
  FLOOR_TEST_NUM_EXITS, floor_test_exits,     // # of exits, exits
  FLOOR_TEST_NUM_CHESTS, floor_test_chests,   // # of chests, chests
  FLOOR_TEST_NUM_SIGNS, floor_test_signs,     // # signs, signs
  floor_test_on_init,
  floor_test_on_update,
  floor_test_on_draw,
  floor_test_on_action,
  floor_test_before_chest,
  floor_test_on_chest,
  floor_test_on_enter,
  floor_test_on_special,
  floor_test_on_exit,
  floor_test_on_move,
};

