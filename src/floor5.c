#pragma bank 8

#include "floor.h"
#include "monster.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 12
#define DEFAULT_Y 30

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_five_data, 32, 32 },
  { MAP_B, BANK_16, floor_five_sub_data, 23, 7 },

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

  // Maze Chests
  { CHEST_1, MAP_A, 23, 11, false, false, str_chest_item_1remedy, chest_item_1remedy },
  { CHEST_2, MAP_A, 21, 18, false, false, str_chest_item_3potions, chest_item_3potions },
  { CHEST_3, MAP_A, 6, 16, false, false, NULL, NULL, chest_add_magic_key },
  { CHEST_4, MAP_A, 6, 30, false, false, NULL, NULL, chest_add_magic_key },

  // Treasure Room Chests
  { CHEST_5, MAP_B, 17, 3, true, true, str_chest_item_3ethers, chest_item_3ethers },
  { CHEST_6, MAP_B, 19, 2, true, true, str_chest_item_1elixir, chest_item_1elixir },
  { CHEST_7, MAP_B, 21, 3, true, true, str_chest_item_1atkup_1defup, chest_item_1atkup_1defup },

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
  // Boss
  { MAP_A, 3, 18, MAP_B, 3, 5, UP, EXIT_STAIRS },
  { MAP_B, 3, 5, MAP_A, 3, 18, DOWN, EXIT_STAIRS },

  // Elite
  { MAP_A, 26, 6, MAP_B, 11, 5, UP, EXIT_STAIRS },
  { MAP_B, 11, 5, MAP_A, 26, 6, DOWN, EXIT_STAIRS },

  // Item Room
  { MAP_A, 2, 9, MAP_B, 19, 5, UP, EXIT_STAIRS },
  { MAP_B, 19, 5, MAP_A, 2, 9, DOWN, EXIT_STAIRS },

  // Next Level
  { MAP_B, 3, 1, MAP_A, 8, 7, UP, EXIT_STAIRS, &bank_floor6 },
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
  { MAP_A, 12, 27, UP, str_floor5_demands }, // Cryptic entry message
  { MAP_A, 20, 29, UP, str_floor5_secrets }, // Cryptic secret message

  // { MAP_A,  2, 18, UP, str_floor_common_tbd }, // Boss
  // { MAP_A, 13, 14, UP, str_floor_common_tbd }, // Entrance
  // { MAP_A, 25,  6, UP, str_floor_common_tbd }, // Elite

  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

/**
 * Holds the state of the flame above the first lever for the boss door puzzle.
 */
FlameColor lever1_flame = FLAME_NONE;

/**
 * Holds the state of the flame above the second lever for the boss door puzzle.
 */
FlameColor lever2_flame = FLAME_NONE;

/**
 * Holds the state of the flame above the third lever for the boss door puzzle.
 */
FlameColor lever3_flame = FLAME_NONE;

static void on_lever_pulled(const Lever *lever) {
  switch (lever->id) {
  case LEVER_1:
    lever1_flame++;
    if (lever1_flame > FLAME_BLUE)
      lever1_flame = FLAME_RED;
    light_sconce(SCONCE_1, lever1_flame);
    break;
  case LEVER_2:
    lever2_flame++;
    if (lever2_flame > FLAME_BLUE)
      lever2_flame = FLAME_RED;
    light_sconce(SCONCE_2, lever2_flame);
    break;
  case LEVER_3:
    lever3_flame++;
    if (lever3_flame > FLAME_BLUE)
      lever3_flame = FLAME_RED;
    light_sconce(SCONCE_3, lever3_flame);
    break;
  }


  if (
    lever1_flame == FLAME_RED &&
    lever2_flame == FLAME_GREEN &&
    lever3_flame == FLAME_BLUE
  ) {
    play_sound(sfx_big_door_open);
    open_door(DOOR_3);
    map_textbox(str_floor2_door_opens);
  } else {
    play_sound(sfx_door_unlock);
    close_door(DOOR_3);
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

  // Skull Lever (needs red)
  { LEVER_1, MAP_A, 10, 12, false, false, on_lever_pulled },

  // Potion Lever (needs green)
  { LEVER_2, MAP_A, 2, 3, false, false, on_lever_pulled },

  // Boss Lever (needs blue)
  { LEVER_3, MAP_A, 27, 20, false, false, on_lever_pulled },

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

  // Next Level Door
  { DOOR_1, MAP_B,  3, 1, DOOR_NEXT_LEVEL, false },

  // Item Room Door
  { DOOR_2, MAP_A, 2, 9, DOOR_STAIRS_DOWN, false, false },

  // Boss Room Door
  { DOOR_3, MAP_A, 3, 18, DOOR_STAIRS_DOWN, false, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

static void on_lit(const Sconce* sconce) {
  if (sconce->id == SCONCE_4) {
    // Open the door to the item room
    open_door(DOOR_2);
    play_sound(sfx_big_door_open);
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

  // Puzzle Sconces
  { SCONCE_1, MAP_A, 10, 10, false },
  { SCONCE_2, MAP_A, 2, 1, false },
  { SCONCE_3, MAP_A, 27, 18, false },

  // Lightable Maze Sconces
  { SCONCE_4, MAP_A, 3, 9, false, FLAME_NONE, on_lit },
  { SCONCE_5, MAP_A, 13, 12, false },
  { SCONCE_6, MAP_A, 7, 11, false },
  { SCONCE_7, MAP_A, 22, 7, false },
  { SCONCE_8, MAP_A, 30, 7, false },

  // Signpost Sconces in Entryway
  { SCONCE_STATIC, MAP_A, 11, 26, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 12, 26, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 13, 26, true, FLAME_BLUE },

  // Static Maze Sconces
  { SCONCE_STATIC, MAP_A, 5, 1, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 14, 17, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 8, 27, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 16, 27, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 28, 27, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 19, 1, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 28, 11, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 4, 18, true, FLAME_BLUE },

  // Static Boss Room
  { SCONCE_STATIC, MAP_B, 2, 1, true, FLAME_RED },
  { SCONCE_STATIC, MAP_B, 4, 1, true, FLAME_RED },

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
  grant_ability(ABILITY_4);
  play_sound(sfx_big_powerup);
  map_textbox(get_grant_message(ABILITY_4));
}

static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  deathknight_generator(monster, 39, B_TIER);
  monster->id = 'A';
  set_on_victory(on_boss_victory);
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  gelatinous_cube_generator(monster, 37, B_TIER);
  monster->id = 'A';
  set_on_victory(on_elite_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    if (player.level < 40) {
      map_textbox(str_floor5_boss_not_yet);
      return true;
    }
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor5_boss, on_boss_encouter);
    return true;
  case NPC_2:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor5_elite_attack, on_elite_encouter);
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
  { NPC_1, MAP_B, 3, 3, MONSTER_DEATHKNIGHT, S_TIER, on_npc_action }, // Boss
  { NPC_2, MAP_B, 11, 3, MONSTER_GELATINOUS_CUBE, A_TIER, on_npc_action }, // Elite

  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

/*
G. Cube
Owlbear
Bugbear
Zombie
*/

// Max Level: 36
static const EncounterTable encounters_low[] = {
  {
    ODDS_10P, MONSTER_LAYOUT_1,
    MONSTER_GELATINOUS_CUBE, 30, B_TIER,
  },
  {
    ODDS_20P, MONSTER_LAYOUT_2,
    MONSTER_BUGBEAR, 32, C_TIER,
    MONSTER_BUGBEAR, 32, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_1,
    MONSTER_OWLBEAR, 30, B_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 30, C_TIER,
    MONSTER_ZOMBIE, 32, C_TIER,
  },
  { END }
};

// Max Level: 42
static const EncounterTable encounters_high[] = {
  {
    ODDS_25P, MONSTER_LAYOUT_2,
    MONSTER_GELATINOUS_CUBE, 34, C_TIER,
    MONSTER_GELATINOUS_CUBE, 34, C_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_1,
    MONSTER_OWLBEAR, 38, B_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 36, C_TIER,
    MONSTER_ZOMBIE, 36, B_TIER,
  },
  {
    ODDS_15P, MONSTER_LAYOUT_3S,
    MONSTER_GOBLIN, 39, C_TIER,
    MONSTER_GOBLIN, 38, B_TIER,
    MONSTER_GOBLIN, 39, C_TIER,
  },
  { END }
};

static bool on_init(void) {
  config_random_encounter(4, 1, 1, true);

  // Reset the boss door flame puzzle
  lever1_flame = FLAME_NONE;
  lever2_flame = FLAME_NONE;
  lever3_flame = FLAME_NONE;

  return false;
}

static bool on_special(void) {
  if (player_at(6, 27)) {
    map_textbox(str_floor_common_strange_wind);
    return true;
  }
  return false;
}

static bool on_move(void) {
  if (check_random_encounter()) {
    if (player.level < 37)
      generate_encounter(encounters_low);
    else
      generate_encounter(encounters_high);
    start_battle();
    return true;
  }
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
  RGB8(80, 0, 80),
  RGB8(50, 0, 60),
  RGB8(24, 0, 0),
  RGB8(0, 0, 24),
  // Palette 2 - Treasure chests
  RGB8(172, 128, 20),
  RGB8(50, 0, 60),
  RGB8(24, 0, 0),
  RGB8(0, 0, 24),
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
const Floor floor5 = {
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
