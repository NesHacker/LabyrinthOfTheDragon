#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 24
#define DEFAULT_Y 30

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_four_data, 32, 32 },

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
  // Secret 1
  {
    CHEST_1,
    MAP_A, 12, 28, false, false,
    str_chest_item_1pots,
    chest_item_1pot,
  },
  // Secret 2
  {
    CHEST_2,
    MAP_A, 3, 28, false, false,
    str_chest_item_1eths,
    chest_item_1eth,
  },
  // West Wing Chest
  {
    CHEST_3,
    MAP_A, 2, 13, false, false,
    NULL, NULL,
    chest_add_magic_key
  },
  // East Wing Chest
  {
    CHEST_4,
    MAP_A, 18, 13, false, false,
    str_chest_item_1eth,
    chest_item_1eth,
  },

  // Treasure Room
  {
    CHEST_5,
    MAP_A, 18, 3, true, true,
    str_chest_item_regen_pot,
    chest_item_regen_pot,
  },
  {
    CHEST_6,
    MAP_A, 20, 3, true, true,
    str_chest_item_haste_pot,
    chest_item_haste_pot,
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
    DEST_MAP      // Id of the destination map
    0, 0,         // Column and row
    UP,           // Way the player should be facing leaving the exit
    EXIT_STAIRS,  // Type of exit (not sure if we'll use this yet)
    &bank_floor2  // (optional) bank reference for exits between floors
  },
  */

  // Boss door
  { MAP_A, 28, 27, MAP_A, 28, 23, UP, EXIT_STAIRS },
  { MAP_A, 28, 23, MAP_A, 28, 27, DOWN, EXIT_STAIRS },

  // West Wing
  { MAP_A, 19, 27, MAP_A,  5,  9, DOWN, EXIT_STAIRS },
  { MAP_A,  5,  9, MAP_A, 19, 27, DOWN, EXIT_STAIRS },

  // Elite Room
  { MAP_A, 1, 8, MAP_A, 1, 5, UP, EXIT_STAIRS },
  { MAP_A, 1, 5, MAP_A, 1, 8, DOWN, EXIT_STAIRS },

  // Central Hall
  { MAP_A, 20, 27, MAP_A, 10, 12, UP, EXIT_STAIRS },
  { MAP_A, 10, 12, MAP_A, 20, 27, DOWN, EXIT_STAIRS },

  // East Wing
  { MAP_A, 21, 27, MAP_A, 15,  9, DOWN, EXIT_STAIRS },
  { MAP_A, 15,  9, MAP_A, 21, 27, DOWN, EXIT_STAIRS },

  // Treasure Room
  { MAP_A, 19, 8, MAP_A, 19, 5, UP, EXIT_STAIRS },
  { MAP_A, 19, 5, MAP_A, 19, 8, DOWN, EXIT_STAIRS },

  // Floor Exit
  // TODO Map this to floor 5
  { MAP_A, 28, 19, MAP_A, 12, 30, UP, EXIT_STAIRS, &bank_floor5 },

  { END },
};

//------------------------------------------------------------------------------
// Signs
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
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

static void on_pull(const Lever *lever) {
  (void)lever;

  const bool lever1 = is_lever_on(LEVER_1);
  const bool lever2 = is_lever_on(LEVER_2);
  const bool lever3 = is_lever_on(LEVER_3);


  if (!lever1 && lever2 && lever3) {
    play_sound(sfx_big_powerup);
    open_door(DOOR_3);
    open_door(DOOR_4);
    map_textbox(str_floor2_door_opens);
  } else {
    play_sound(sfx_door_unlock);
    close_door(DOOR_3);
    close_door(DOOR_4);
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

  // West Wing
  { LEVER_1, MAP_A, 1, 17, false, false, on_pull },

  // Central Hall
  { LEVER_2, MAP_A, 10, 16, false, false, on_pull },

  // East Wing
  { LEVER_3, MAP_A, 19, 17, false, false, on_pull },

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

  // Boss Door
  { DOOR_1, MAP_A, 28, 27, DOOR_NORMAL, false, false },

  // Next Floor
  { DOOR_2, MAP_A, 28, 19, DOOR_NEXT_LEVEL, false, false },

  // Elite Door (West Wing)
  { DOOR_3, MAP_A, 1, 8, DOOR_NORMAL, false, false },

  // Treasure Room Door (East Wing)
  { DOOR_4, MAP_A, 19, 8, DOOR_NORMAL, false, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

/**
 * Keeps track of the number of sconce puzzles (out of 3) that have been solved.
 */
uint8_t puzzle_count = 0;

/**
 * Handles the logic for the three color flame puzzle.
 */
static void test_sconces(SconceId a, SconceId b, FlameColor c) {
  if (is_sconce_lit(a) && is_sconce_lit(b)) {
    if (get_sconce_color(a) == c && get_sconce_color(b) == c) {
      puzzle_count++;
      if (puzzle_count >= 3) {
        play_sound(sfx_monster_critical);
        map_textbox(str_floor2_door_opens);
        open_door(DOOR_1);
      } else {
        play_sound(sfx_big_powerup);
      }
    }
    else {
      play_sound(sfx_error);
      extinguish_sconce(a);
      extinguish_sconce(b);
    }
  }
}

static void on_lit(const Sconce* sconce) {
  switch (sconce->id) {
  case SCONCE_1:
  case SCONCE_2:
    test_sconces(SCONCE_1, SCONCE_2, FLAME_GREEN);
    break;
  case SCONCE_3:
  case SCONCE_4:
    test_sconces(SCONCE_3, SCONCE_4, FLAME_RED);
    break;
  case SCONCE_5:
  case SCONCE_6:
    test_sconces(SCONCE_5, SCONCE_6, FLAME_BLUE);
    break;
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

  // West Wing
  { SCONCE_1, MAP_A, 2, 9, false, FLAME_NONE, on_lit },
  { SCONCE_2, MAP_A, 4, 9, false, FLAME_NONE, on_lit },

  // Central Hall
  { SCONCE_3, MAP_A, 9, 9, false, FLAME_NONE, on_lit },
  { SCONCE_4, MAP_A, 10, 9, false, FLAME_NONE, on_lit },

  // East Wing
  { SCONCE_5, MAP_A, 17, 9, false, FLAME_NONE, on_lit },
  { SCONCE_6, MAP_A, 18, 9, false, FLAME_NONE, on_lit },

  // Main Room (colored sconces)
  { SCONCE_STATIC, MAP_A, 23, 27, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 24, 27, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 25, 27, true, FLAME_RED },

  // Treasure & Elite Room
  { SCONCE_STATIC, MAP_A, 1, 2, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 19, 2, true, FLAME_RED },

  // Boss Room
  { SCONCE_STATIC, MAP_A, 27, 19, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 29, 19, true, FLAME_RED },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------

static void on_boss_victory(void) BANKED {
  open_door(DOOR_2);
  set_npc_invisible(NPC_1);
  play_sound(sfx_big_door_open);
}

static void on_elite_victory(void) BANKED {
  set_npc_invisible(NPC_2);
  grant_ability(ABILITY_3);
  play_sound(sfx_big_powerup);
  map_textbox(get_grant_message(ABILITY_3));
}

static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  displacer_beast_generator(monster, 31, A_TIER);
  monster->id = 'A';
  set_on_victory(on_boss_victory);
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  owlbear_generator(monster, 29, B_TIER);
  monster->id = 'A';
  set_on_victory(on_elite_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    if (player.level < 29) {
      map_textbox(str_floor4_boss_not_yet);
      return true;
    }
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor4_boss, on_boss_encouter);
    return true;
  case NPC_2:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor4_elite_attack, on_elite_encouter);
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

  { NPC_1, MAP_A, 28, 21, MONSTER_DISPLACER_BEAST, S_TIER, on_npc_action },
  { NPC_2, MAP_A, 1, 3, MONSTER_OWLBEAR, A_TIER, on_npc_action },

  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

/*
Owlbear
Bugbear
Zombie
Goblin
*/

// Max Level: 29
static const EncounterTable encounters_low[] = {
  {
    ODDS_10P, MONSTER_LAYOUT_1,
    MONSTER_OWLBEAR, 25, B_TIER,
  },
  {
    ODDS_20P, MONSTER_LAYOUT_1,
    MONSTER_BUGBEAR, 24, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_GOBLIN, 25, C_TIER,
    MONSTER_GOBLIN, 25, B_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 19, C_TIER,
    MONSTER_ZOMBIE, 18, C_TIER,
  },
  { END }
};

// Max Level: 33
static const EncounterTable encounters_high[] = {
  {
    ODDS_20P, MONSTER_LAYOUT_2,
    MONSTER_OWLBEAR, 27, C_TIER,
    MONSTER_OWLBEAR, 29, C_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_BUGBEAR, 31, B_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 29, C_TIER,
    MONSTER_ZOMBIE, 29, C_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_3S,
    MONSTER_GOBLIN, 28, C_TIER,
    MONSTER_GOBLIN, 29, B_TIER,
    MONSTER_GOBLIN, 28, C_TIER,
  },
  { END }
};

static bool on_init(void) {
  puzzle_count = 0;
  config_random_encounter(4, 1, 1, true);
  return false;
}

static bool on_special(void) {
  return false;
}

static bool on_move(void) {
  if (check_random_encounter()) {
    if (player.level < 29)
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
  RGB8(175, 160, 177),
  RGB8(110, 70, 110),
  RGB8(40, 50, 40),
  RGB8(32, 0, 32),
  // Palette 2 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(110, 70, 110),
  RGB8(40, 50, 40),
  RGB8(32, 0, 32),
  // Palette 3 - Levers
  RGB8(192, 40, 40),
  RGB8(110, 70, 110),
  RGB8(40, 50, 40),
  RGB8(32, 0, 32),
  // Palette 4 - Special Skull
  RGB8(20, 100, 177),
  RGB8(110, 70, 110),
  RGB8(40, 50, 40),
  RGB8(32, 0, 32),
  // Palette 5 - Regen Chest
  RGB8(20, 177, 100),
  RGB8(110, 70, 110),
  RGB8(40, 50, 40),
  RGB8(32, 0, 32),
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
const Floor floor4 = {
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
