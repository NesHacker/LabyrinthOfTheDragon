#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define FLOOR_TEST_ID 99
#define FLOOR_TEST_START_COL 0
#define FLOOR_TEST_START_ROW 0

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

#define FLOOR_TEST_NUM_MAPS 1

const Map floor_test_maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_9, floor_test_data, 32, 32 },
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------
const uint16_t floor_test_palettes[] = {
  // Palette 0 - Core background tiles
  RGB8(190, 200, 190),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 1 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 2
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
  // Palette 3
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
};

//------------------------------------------------------------------------------
// Chests
//------------------------------------------------------------------------------
#define FLOOR_TEST_NUM_CHESTS 0

typedef enum floor_testChests {
  F1_CHEST_MyChesticles,
} floor_testChests;

const Chest floor_test_chests[] = {
  /*
  {
    1,                // id (anything non-zero here works, ptrs amirite?)
    MAP_A,            // Map where the chest is at
    0, 0,             // Column & row in that map
    FLAGS_CHEST_OPEN, // Flag page for the open/closed flag
    CHEST_1           // Open flag
  }
  */
  { 0 }, // Remove me when you add your first chest (pointers amirite?)
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------
#define FLOOR_TEST_NUM_EXITS 0

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
  { 0 }, // Remove me when you add your first chest (pointers amirite?)
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

void floor_test_on_action(void) {
}

bool floor_test_before_chest(Chest *chest) {
  return true;
}

void floor_test_on_enter(uint8_t from_id, uint8_t to_id) {
}

void floor_test_on_chest(Chest *chest) {
  switch (chest->id) {
  case F1_CHEST_MyChesticles:
    break;
  }
}

bool floor_test_on_special(void) {
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
const Area floor_test = {
  FLOOR_TEST_ID,                              // Id
  FLOOR_TEST_START_COL, FLOOR_TEST_START_ROW, // Starting Col, Row
  floor_test_palettes,                        // Palettes
  FLOOR_TEST_NUM_MAPS, floor_test_maps,       // # of maps, maps
  FLOOR_TEST_NUM_EXITS, floor_test_exits,     // # of exits, exits
  FLOOR_TEST_NUM_CHESTS, floor_test_chests,   // # of chests, chests
  BANK_2,
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

