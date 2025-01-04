#pragma bank 8

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 7
#define DEFAULT_Y 30

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_one_data, 32, 32 },

  { END },
};

//------------------------------------------------------------------------------
// Chests
//------------------------------------------------------------------------------

static const Chest chests[] = {
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
    MAP_A, 21, 18, false, false,
    NULL, NULL,
    chest_add_torch,
  },
  {
    CHEST_2,
    MAP_A, 23, 26, false, false,
    NULL, NULL,
    chest_add_magic_key,
  },
  {
    CHEST_3,
    MAP_A, 22, 9, false, false,
    str_chest_item_1pots,
    chest_item_1pot,
  },

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

    {MAP_A, 6, 3, MAP_A, 4, 19, UP, EXIT_STAIRS, &bank_floor2},

    {END},
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

static const Sign signs[] = {
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

static const Door doors[] = {
  /*
  {
    DOOR_1,           // Use DOOR_* constants for ids.
    MAP_A,            // Map for the door
    0, 0              // (x, y) tile for the door
    DOOR_STAIRS_UP    // Kind of door
    true              // Magic key required to unlock
    false,            // Does the door start opened?
  }
  */
  { DOOR_1, MAP_A, 3, 25, DOOR_STAIRS_DOWN, true, false },
  { DOOR_2, MAP_A, 6, 18, DOOR_NORMAL, false, false },
  { DOOR_3, MAP_A, 6,  3, DOOR_NEXT_LEVEL, false, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

static void on_lit(const Sconce* sconce) {
  if(is_sconce_lit(SCONCE_1) && is_sconce_lit(SCONCE_2)) {
    //open the door dave
    open_door(DOOR_2);
  }
}

static const Sconce sconces[] = {
  /*
  {
    SCONCE_1,   // Use SCONCE_* constants for ids.
    MAP_A,      // Map for the sconce
    0, 0,       // (x, y) tile for the sconce
    true,       // Should the sconce start lit
    FLAME_BLUE  // Flame color for the sconce if it starts lit.
  }
  */
  { SCONCE_1, MAP_A, 4, 18, false, FLAME_RED, on_lit },
  { SCONCE_2, MAP_A, 8, 18, false, FLAME_RED, on_lit },

  // These do nothing Jon Snow
  { SCONCE_3, MAP_A,  5, 25, true, FLAME_RED },
  { SCONCE_4, MAP_A, 20, 17, true, FLAME_RED },
  { SCONCE_5, MAP_A, 22, 17, true, FLAME_RED },
  { SCONCE_6, MAP_A, 22,  6, true, FLAME_RED },


  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------

static void boss_victory(void) NONBANKED {
  open_door(DOOR_3);
  set_npc_invisible(NPC_1);
}

static bool boss_encounter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  kobold_generator(monster, player.level, A_TIER);
  monster->id = 'A';
  set_on_victory(boss_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  map_textbox_with_action(str_floor_common_growl, boss_encounter);
  return true;
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
  { NPC_1, MAP_A, 6, 6, MONSTER_KOBOLD, on_npc_action },

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
const Floor floor1 = {
  ID, DEFAULT_X, DEFAULT_Y, palettes,
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

