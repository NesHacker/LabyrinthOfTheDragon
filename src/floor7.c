#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 8
#define DEFAULT_Y 31

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_seven_data, 32, 32 },
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

  // Left & Right Wing Magic Key Chests
  { CHEST_1, MAP_A, 2, 1, false, false, NULL, NULL, chest_add_magic_key },
  { CHEST_2, MAP_A, 18, 1, false, false, NULL, NULL, chest_add_magic_key },

  // Left & Right Wing Item Chests
  { CHEST_3, MAP_A, 6, 5, false, false, str_chest_item_haste_pot, chest_item_haste_pot },
  { CHEST_4, MAP_A, 14, 5, false, false, str_chest_item_regen_pot, chest_item_regen_pot },

  // Item Room Chest
  {
    CHEST_5, MAP_A, 10, 17,
    false, false,
    str_chest_item_3elixers, chest_item_3elixers,
    floor7_chest_on_open
  },

  // Secret Maze Chests
  {
    CHEST_6, MAP_A, 25, 28,
    false, false,
    str_chest_item_regen_pot, chest_item_regen_pot,
    floor7_chest_on_open
  },
  { CHEST_7, MAP_A, 30, 22, false, false, str_chest_item_regen_pot, chest_item_regen_pot },
  { CHEST_8, MAP_A, 25, 18, false, false, str_chest_item_1pots, chest_item_1pot },

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
    DEST_MAP      // Id of the destination map
    0, 0,         // Column and row
    UP,           // Way the player should be facing leaving the exit
    EXIT_STAIRS   // Type of exit (not sure if we'll use this yet)
  },
  */
  // Boss Door
  { MAP_A, 8, 27, MAP_A, 27, 9, UP, EXIT_STAIRS },
  { MAP_A, 27, 9, MAP_A, 8, 27, DOWN, EXIT_STAIRS },

  // Elite Room
  { MAP_A, 7, 27, MAP_A, 2, 19, UP, EXIT_STAIRS },
  { MAP_A, 2, 19, MAP_A, 7, 27, DOWN, EXIT_STAIRS },

  // Item Room
  { MAP_A, 9, 27, MAP_A, 10, 19, UP, EXIT_STAIRS },
  { MAP_A, 10, 19, MAP_A, 9, 27, DOWN, EXIT_STAIRS },

  // Left Wing Portal & Hole
  { MAP_A, 1, 29, MAP_A, 7, 11, HERE, EXIT_PORTAL },
  { MAP_A, 9, 6, MAP_A, 7, 30, HERE, EXIT_HOLE },

  // Right Wing Portal & Hole
  { MAP_A, 15, 29, MAP_A, 13, 11, HERE, EXIT_PORTAL },
  { MAP_A, 11, 6, MAP_A, 9, 30, HERE, EXIT_HOLE },

  // Secret Maze
  { MAP_A, 24, 7, MAP_A, 31, 29, LEFT, EXIT_STAIRS },
  { MAP_A, 31, 29, MAP_A, 24, 7, RIGHT, EXIT_STAIRS },

  // Next Floor
  { MAP_A, 27, 5, MAP_A, DEFAULT_X, DEFAULT_Y, UP, EXIT_STAIRS, &bank_floor7 },
  { END },
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
  { MAP_A, 10, 27, UP, str_floor7_riddle },
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

#define START_STUCK false

static void on_pulled(const Lever *lever) {
  // switch (lever->id) {
  // case LEVER_1:
  //   break;
  // case LEVER_2:
  //   break;
  // }
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
  { LEVER_1, MAP_A, 7, 29, false, START_STUCK, on_pulled },
  { LEVER_2, MAP_A, 9, 29, false, START_STUCK, on_pulled },
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
  { DOOR_1, MAP_A, 27, 5, DOOR_NEXT_LEVEL, false, false },  // Next Level
  { DOOR_2, MAP_A, 8, 27, DOOR_NEXT_LEVEL, false, false },  // Boss
  { DOOR_3, MAP_A, 7, 27, DOOR_STAIRS_DOWN, false, false }, // Elite
  { DOOR_4, MAP_A, 9, 27, DOOR_STAIRS_DOWN, false, false }, // Item Room
  { DOOR_5, MAP_A, 7, 9, DOOR_NORMAL, true, false },   // Left Wing Locked
  { DOOR_6, MAP_A, 2, 4, DOOR_NORMAL, false, false },  // Left Wing Switch
  { DOOR_7, MAP_A, 13, 9, DOOR_NORMAL, true, false },  // Right Wing Locked
  { DOOR_8, MAP_A, 18, 4, DOOR_NORMAL, false, false }, // Right Wing Switch
  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

static void on_lit(const Sconce* sconce) {
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
  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------
static void on_boss_victory(void) BANKED {
  open_door(DOOR_1);
  set_npc_invisible(NPC_1);
  play_sound(sfx_big_door_open);
}

static void on_elite_victory(void) BANKED {
  set_npc_invisible(NPC_2);
  add_items(ITEM_HASTE, 1);
  play_sound(sfx_big_powerup);
  map_textbox(str_chest_item_haste_pot);
}

static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  beholder_generator(monster, 54, A_TIER);
  monster->id = 'A';
  set_on_victory(on_boss_victory);
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  displacer_beast_generator(monster, 50, B_TIER);
  monster->id = 'A';
  set_on_victory(on_elite_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    if (player.level < 45) {
      map_textbox(str_floor6_boss_not_yet);
      return true;
    }
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor6_boss, on_boss_encouter);
    return true;
  case NPC_2:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor6_elite_attack, on_elite_encouter);
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
    B_TIER,           // Tier for the monster (determines palette)
    action_callback,  // Action callback to execute when the player interacts
  }
  */
  // { NPC_1, MAP_A, 6, 6, MONSTER_KOBOLD, on_npc_action },
  { NPC_1, MAP_A, 27, 7, MONSTER_BEHOLDER, S_TIER, on_npc_action }, // Boss
  { NPC_2, MAP_A, 2, 17, MONSTER_DISPLACER_BEAST, A_TIER, on_npc_action }, // Elite
  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

static bool on_init(void) {
  return false;
}

static bool on_special(void) {
  return false;
}

static bool on_move(void) {
  return false;
}

static bool on_action(void) {
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
const Floor floor7 = {
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
  on_action,
};
