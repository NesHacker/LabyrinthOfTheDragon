#pragma bank 31

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------
#define ID 98
#define DEFAULT_X 2
#define DEFAULT_Y 14
#define DEFAULT_MAP MAP_A

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  { MAP_A, BANK_31, map_example_0, 16, 16 },
  { MAP_B, BANK_31, map_example_1, 16, 16 },
  { MAP_C, BANK_31, map_example_11x13, 11, 13 },
  { END },
};

//------------------------------------------------------------------------------
// Chests
//------------------------------------------------------------------------------

static const Chest chests[] = {
  { CHEST_1, MAP_A, 9, 13, false, false, NULL, NULL, chest_add_torch },
  { END },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

static const Exit exits[] = {
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
  { MAP_C, 5, 2, MAP_A, 4, 4, UP, EXIT_STAIRS, &bank_floor_test },
  { END },
};

// 5, 12

//------------------------------------------------------------------------------
// Signs
//------------------------------------------------------------------------------

static const Sign signs[] = {
  /*
  {
    MAP_A,      // Id of the map
    0, 0,       // Position in the map for the sign
    UP,         // Direction the player must be facing
    "Hi there!" // The message to display (needs to be accessible on bank 2)
  }
  */
  { MAP_A, 2, 15, DOWN, str_floor_test_no_back },
  { MAP_A, 3, 15, DOWN, str_floor_test_no_back },
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

static const Lever levers[] = {
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

const Door doors[] = {
  { DOOR_1, MAP_A, 11, 2, DOOR_NEXT_LEVEL, false },
  { DOOR_2, MAP_C, 5, 2, DOOR_NEXT_LEVEL, false },
  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

void on_lit(const Sconce *sconce) {
  if (sconce->id == SCONCE_2 || sconce->id == SCONCE_3) {
    if (
      is_sconce_lit(SCONCE_2) &&
      is_sconce_lit(SCONCE_3) &&
      !is_door_open(DOOR_1)
    ) {
      open_door(DOOR_1);
    }
  }
}

static const Sconce sconces[] = {
  { SCONCE_STATIC, MAP_A, 6, 4, true, FLAME_RED },
  { SCONCE_STATIC, MAP_C, 4, 2, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_C, 6, 2, true, FLAME_BLUE },
  { SCONCE_2, MAP_A, 10, 2, false, FLAME_NONE, on_lit },
  { SCONCE_3, MAP_A, 12, 2, false, FLAME_NONE, on_lit },
  { END }
};

//------------------------------------------------------------------------------
// NPCs
//------------------------------------------------------------------------------

void boss_victory(void) NONBANKED {
  open_door(DOOR_2);
  set_npc_invisible(NPC_1);
}

static bool boss_encounter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  kobold_generator(monster, 20, B_TIER);
  monster->id = 'A';
  set_on_victory(boss_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    map_textbox_with_action(str_floor_test_growl, boss_encounter);
    return true;
  }
  return false;
}

static const NPC npcs[] = {
  /*
  {
    NPC_1,            // Use NPC_* constants for ids.
    MAP_A,            // Map for the npc
    0, 0              // (x, y) tile for the npc
    MONSTER_KOBOLD,   // Monster graphic to use for the NPC
    action_callback,  // Action callback to execute when the player interacts
  }
  */
  { NPC_1, MAP_C, 5, 5, MONSTER_KOBOLD, on_npc_action },
  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

static const EncounterTable random_encounters[] = {
  {
    ODDS_25P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 6, C_TIER,
    MONSTER_KOBOLD, 5, C_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_GOBLIN, 7, C_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_KOBOLD, 7, C_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_KOBOLD, 5, C_TIER,
  },
  { END }
};

static bool on_init(void) {
  config_random_encounter(6, 1, 1, true);
  return false;
}

static bool on_special(void) {
  return false;
}

static bool on_move(void) {
  if (check_random_encounter()) {
    generate_encounter(random_encounters);
    start_battle();
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

static const palette_color_t palettes[] = {
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
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_test2 = {
  ID, DEFAULT_X, DEFAULT_Y,
  palettes,
  maps,
  exits,
  chests,
  signs,
  levers,
  doors,
  sconces,
  npcs,
  on_init,
  on_special,
  on_move,
};


