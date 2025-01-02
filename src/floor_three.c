#pragma bank 2

#include "floor.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 98
#define DEFAULT_X 5
#define DEFAULT_Y 30
#define DEFAULT_MAP MAP_A

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_three_data, 32, 32 },

  { END },
};

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
    MAP_A, 15, 12, true, true,
    str_chest_item_2pot_1eth,
    chest_item_2pot_1eth,
  },
  {
    CHEST_2,
    MAP_A, 18, 13, false, false,
    str_chest_item_1pots,
    chest_item_1pot,
  },
  {
    CHEST_3,
    MAP_A, 3, 12, false, false,
    NULL,
    NULL,
    chest_add_magic_key,
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
      ID,           // Floor to which the exit leads (last door, basically)
      DEST_MAP      // Id of the destination map
      0, 0,         // Column and row
      UP,           // Way the player should be facing leaving the exit
      EXIT_STAIRS   // Type of exit (not sure if we'll use this yet)
    },
    */

    {MAP_A, 6, 26, MAP_A, 3, 17, DOWN, EXIT_STAIRS},
    {MAP_A, 3, 17, MAP_A, 6, 26, DOWN, EXIT_STAIRS},

    {MAP_A, 10, 17, MAP_A, 10, 13, UP, EXIT_STAIRS},
    {MAP_A, 10, 13, MAP_A, 10, 17, DOWN, EXIT_STAIRS},

    {MAP_A, 5, 6, MAP_A, 4, 11, DOWN, EXIT_STAIRS},
    {MAP_A, 4, 11, MAP_A, 5, 6, DOWN, EXIT_STAIRS},

    {MAP_A, 16, 6, MAP_A, 21, 6, DOWN, EXIT_STAIRS},
    {MAP_A, 21, 6, MAP_A, 16, 6, DOWN, EXIT_STAIRS},

    {MAP_A, 27, 9, MAP_A, 14, 11, DOWN, EXIT_STAIRS},
    {MAP_A, 14, 11, MAP_A, 27, 9, DOWN, EXIT_STAIRS},

    {MAP_A, 28, 9, MAP_A, 15, 18, DOWN, EXIT_STAIRS},
    {MAP_A, 15, 18, MAP_A, 28, 9, DOWN, EXIT_STAIRS},

    {MAP_A, 29, 9, MAP_A, 29, 23, DOWN, EXIT_STAIRS},
    {MAP_A, 29, 23, MAP_A, 29, 9, DOWN, EXIT_STAIRS},

    {MAP_A, 23, 21, MAP_A, 5, 29, UP, EXIT_STAIRS, &floor_three},

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
  { MAP_A, 13, 6, UP, "Make sure to\n turn off the\n telly-porta!" },
  { MAP_A, 24, 9, UP, "Make sure to\n turn off the\n telly-porta!" },
  { MAP_A, 23, 13, UP, "Make sure to\n turn off the\n telly-porta!" },
  { MAP_A, 27, 13, UP, "Make sure to\n turn off the\n telly-porta!" },
  { MAP_A, 16, 28, UP, "It's so sad\n that Steve Jobs\n Died of Ligma\x60" },

  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

static void lever_check(void) {
  if(is_lever_on(LEVER_8) && is_lever_on(LEVER_7) && is_lever_on(LEVER_6))
    open_door_by_id(DOOR_4);

  if(is_lever_on(LEVER_8) && !is_lever_on(LEVER_7) && is_lever_on(LEVER_6))
    open_door_by_id(DOOR_5);

  if(is_lever_on(LEVER_8) && !is_lever_on(LEVER_7) && !is_lever_on(LEVER_6) && is_lever_on(LEVER_5))
    open_door_by_id(DOOR_6);
}

static void on_lever_pull(const Lever *lever) {
  switch (lever->id)
  {
  case LEVER_1:
    open_door_by_id(DOOR_1);
    break;
  case LEVER_2:
    break;
  case LEVER_3:
    open_door_by_id(DOOR_2);
    break;
  case LEVER_4:
    open_door_by_id(DOOR_3);
    break;
  case LEVER_5:
    lever_check();
    break;
  case LEVER_6:
    lever_check();
    break;
  case LEVER_7:
    lever_check();
    break;
  case LEVER_8:
    lever_check();
    break;
  default:
    break;
  }
}

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
  { LEVER_1, MAP_A,  4, 28, true, false, on_lever_pull },
  { LEVER_2, MAP_A, 12, 28, true, false },
  { LEVER_3, MAP_A,  9,  6, true, false, on_lever_pull },
  { LEVER_4, MAP_A, 11,  6, true, false, on_lever_pull },

  { LEVER_5, MAP_A, 22, 14, false, false, on_lever_pull },
  { LEVER_6, MAP_A, 24, 14, false, false, on_lever_pull },
  { LEVER_7, MAP_A, 26, 14, false, false, on_lever_pull },
  { LEVER_8, MAP_A, 28, 14, false, false, on_lever_pull },

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
  }
  */
  { DOOR_1, MAP_A,  6, 26, DOOR_NORMAL, false },
  { DOOR_2, MAP_A,  5,  6, DOOR_NORMAL, false },
  { DOOR_3, MAP_A, 16,  6, DOOR_NORMAL, false },

  { DOOR_4, MAP_A, 27,  9, DOOR_NORMAL, false },
  { DOOR_5, MAP_A, 28,  9, DOOR_NORMAL, false },
  { DOOR_6, MAP_A, 29,  9, DOOR_NORMAL, false },

  { DOOR_7, MAP_A, 23, 21, DOOR_NEXT_LEVEL, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

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
  { SCONCE_STATIC, MAP_A, 12, 26, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 14, 18, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 16, 18, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 10,  5, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 25, 13, true, FLAME_RED },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (NOT IMPLS YET)
//------------------------------------------------------------------------------

static void on_boss_victory(void) NONBANKED {
  set_door_open(DOOR_7);
  set_npc_invisible(NPC_2);
}

static void on_elite_victory(void) NONBANKED {
  grant_ability(ABILITY_1);
  set_npc_invisible(NPC_1);
  map_textbox("You get a new ability!");
}

static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  kobold_generator(monster, player.level, A_TIER);
  monster->id = 'A';
  set_on_victory(on_boss_victory);
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  kobold_generator(monster, player.level, A_TIER);
  monster->id = 'A';
  set_on_victory(on_elite_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  if (npc->id == NPC_2){
    map_textbox_with_action("FIGHT ME!", on_boss_encouter);
  } else {
    map_textbox_with_action("I LOVE YOU!", on_elite_encouter);
  }
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
  { NPC_1, MAP_A, 15, 20, MONSTER_KOBOLD, on_npc_action },
  { NPC_2, MAP_A, 23, 22, MONSTER_KOBOLD, on_npc_action },

  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

static bool floor_on_init(void) {
  return false;
}

static void floor_on_update(void) {
}

static void floor_on_draw(void) {
}

static bool floor_on_action(void) {
  return false;
}

static bool floor_on_special(void) {
  return false;
}

static bool floor_on_exit(void) {
  return false;
}

static bool floor_on_move(void) {
  return false;
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor_three = {
  ID,                   // Id
  DEFAULT_MAP,          // Default Map
  DEFAULT_X, DEFAULT_Y, // Default Starting (x, y)
  palettes,             // Palettes
  maps,
  exits,
  chests,
  signs,
  levers,
  doors,
  sconces,
  npcs,
  floor_on_init,
  floor_on_update,
  floor_on_draw,
  floor_on_action,
  floor_on_special,
  floor_on_exit,
  floor_on_move,
};
