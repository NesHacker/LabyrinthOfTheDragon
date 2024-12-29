#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define FLOOR_TEST2_ID 98
#define FLOOR_TEST2_DEFAULT_X 2
#define FLOOR_TEST2_DEFAULT_Y 14
#define FLOOR_TEST2_DEFAULT_MAP MAP_A
// #define FLOOR_TEST2_DEFAULT_X 5
// #define FLOOR_TEST2_DEFAULT_Y 12
// #define FLOOR_TEST2_DEFAULT_MAP MAP_C

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

const Map floor_test2_maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_8, map_example_0, 16, 16 },
  { MAP_B, BANK_8, map_example_1, 16, 16 },
  { MAP_C, BANK_8, map_example_11x13, 11, 13 },
  { END },
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

const uint16_t floor_test2_palettes[] = {
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

bool get_torch(const Chest *chest) {
  if (chest->id == 1) {
    map_textbox("You find a torch!");
    player.has_torch = true;
    return true;
  }
  return false;
}

const Chest floor_test2_chests[] = {
  { CHEST_1, MAP_A, 9, 13, false, false, NULL, NULL, get_torch },
  { END },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

const Exit floor_test2_exits[] = {
  /*
  {
    MAP_A,        // Map the exit is on
    0, 0,         // Column and row on that map for the exit
    FLOOR_TEST2_ID,    // Floor to which the exit leads (last door, basically)
    DEST_MAP      // Id of the destination map
    0, 0,         // Column and row
    UP,           // Way the player should be facing leaving the exit
    EXIT_STAIRS   // Type of exit (not sure if we'll use this yet)
  },
  */
  { MAP_A, 2, 2, MAP_B, 2, 2, DOWN, EXIT_STAIRS },
  { MAP_B, 2, 2, MAP_A, 2, 2, DOWN, EXIT_STAIRS },
  { MAP_B, 9, 9, MAP_A, 9, 9, DOWN, EXIT_STAIRS },
  { MAP_A, 9, 9, MAP_B, 9, 9, DOWN, EXIT_STAIRS },
  { MAP_A, 11, 2, MAP_C, 5, 12, UP, EXIT_STAIRS },
  { MAP_C, 5, 2, MAP_A, 4, 4, UP, EXIT_STAIRS, &floor_test },
  { END },
};

// 5, 12

//------------------------------------------------------------------------------
// Signs
//------------------------------------------------------------------------------

const Sign floor_test2_signs[] = {
  /*
  {
    MAP_A,      // Id of the map
    0, 0,       // Position in the map for the sign
    UP,         // Direction the player must be facing
    "Hi there!" // The message to display
  }
  */
  { MAP_A, 2, 15, DOWN, "You can't return\x60" },
  { MAP_A, 3, 15, DOWN, "You can't return\x60" },
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

const Lever floor_test2_levers[] = {
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
  { END },
};

//------------------------------------------------------------------------------
// Doors
//------------------------------------------------------------------------------

const Door floor_test2_doors[] = {
  { DOOR_1, MAP_A, 11, 2, DOOR_NEXT_LEVEL, false },
  { DOOR_2, MAP_C, 5, 2, DOOR_NEXT_LEVEL, false },
  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

void floor_test2_on_lit(const Sconce *sconce) {
  if (sconce->id == SCONCE_2 || sconce->id == SCONCE_3) {
    if (
      is_sconce_lit(SCONCE_2) &&
      is_sconce_lit(SCONCE_3) &&
      !is_door_open(DOOR_1)
    ) {
      open_door_by_id(DOOR_1);
      map_textbox("The door opens!");
    }
  }
}

const Sconce floor_test2_sconces[] = {
  { SCONCE_STATIC, MAP_A, 6, 4, true, FLAME_RED },
  { SCONCE_STATIC, MAP_C, 4, 2, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_C, 6, 2, true, FLAME_BLUE },
  { SCONCE_2, MAP_A, 10, 2, false, FLAME_NONE, floor_test2_on_lit },
  { SCONCE_3, MAP_A, 12, 2, false, FLAME_NONE, floor_test2_on_lit },
  { END }
};

//------------------------------------------------------------------------------
// NPCs
//------------------------------------------------------------------------------

void floor2_boss_victory(void) NONBANKED {
  set_door_open(DOOR_2);
  set_npc_invisible(NPC_1);
}

bool floor2_boss_encounter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  kobold_generator(monster, player.level, A_TIER);
  monster->id = 'A';
  set_on_victory(floor2_boss_victory);
  start_battle();
  return true;
}

bool floor2_on_npc_action(const NPC *npc) {
  map_textbox("Growl\x60");
  map_textbox_with_action("GROWL!", floor2_boss_encounter);
  return true;
}

const NPC floor_test2_npcs[] = {
  /*
  {
    NPC_1,            // Use NPC_* constants for ids.
    MAP_A,            // Map for the npc
    0, 0              // (x, y) tile for the npc
    MONSTER_KOBOLD,   // Monster graphic to use for the NPC
    action_callback,  // Action callback to execute when the player interacts
  }
  */
  { NPC_1, MAP_C, 5, 5, MONSTER_KOBOLD, floor2_on_npc_action },
  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

bool floor_test2_on_init(void) {
  // map_textbox("floor_test2\non_init()");
  return false;
}

void floor_test2_on_update(void) {
}

void floor_test2_on_draw(void) {
}

bool floor_test2_on_action(void) {
  return false;
}
bool floor_test2_on_special(void) {
  // switch (map.active_map->id) {
  // case MAP_A:
  //   break;
  // }
  return false;
}

bool floor_test2_on_exit(void) {
  return false;
}

bool floor_test2_on_move(void) {
  return false;
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_test2 = {
  FLOOR_TEST2_ID,
  FLOOR_TEST2_DEFAULT_MAP,
  FLOOR_TEST2_DEFAULT_X, FLOOR_TEST2_DEFAULT_Y,
  floor_test2_palettes,
  floor_test2_maps,
  floor_test2_exits,
  floor_test2_chests,
  floor_test2_signs,
  floor_test2_levers,
  floor_test2_doors,
  floor_test2_sconces,
  floor_test2_npcs,
  floor_test2_on_init,
  floor_test2_on_update,
  floor_test2_on_draw,
  floor_test2_on_action,
  floor_test2_on_special,
  floor_test2_on_exit,
  floor_test2_on_move,
};


