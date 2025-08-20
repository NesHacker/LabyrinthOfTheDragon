#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 26
#define DEFAULT_Y 17

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_three_data, 32, 32 },
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
    MAP_A, 1, 3, false, false,
    NULL, NULL,
    chest_add_magic_key
  },
  {
    CHEST_2,
    MAP_A, 13, 15, false, false,
    str_chest_item_1pots,
    chest_item_1pot,
  },
  {
    CHEST_3,
    MAP_A, 18, 22, false, false,
    str_chest_item_1eths,
    chest_item_1eth,
  },
  {
    CHEST_4,
    MAP_A, 25, 23, true, true,
    str_chest_item_regen_pot,
    chest_item_regen_pot,
  },
  {
    CHEST_5,
    MAP_A, 29, 23, true, true,
    str_chest_item_1remedy,
    chest_item_1remedy,
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

  { MAP_A, 26, 14, MAP_A, 4, 17, UP, EXIT_STAIRS },
  { MAP_A, 4, 17, MAP_A, 26, 14, DOWN, EXIT_STAIRS },

  { MAP_A, 24, 14, MAP_A, 19, 29, DOWN, EXIT_STAIRS },
  { MAP_A, 19, 29, MAP_A, 24, 14, DOWN, EXIT_STAIRS },

  { MAP_A, 28, 14, MAP_A, 12, 6, DOWN, EXIT_STAIRS },
  { MAP_A, 12, 6, MAP_A, 28, 14, DOWN, EXIT_STAIRS },

  { MAP_A, 14, 29, MAP_A, 27, 28, UP, EXIT_STAIRS },
  { MAP_A, 27, 28, MAP_A, 14, 29, DOWN, EXIT_STAIRS },

  { MAP_A, 17, 6, MAP_A, 3, 5, UP, EXIT_STAIRS },
  { MAP_A, 3, 5, MAP_A, 17, 6, DOWN, EXIT_STAIRS },

  // TODO Fix this to point to floor 4
  { MAP_A, 4, 12, MAP_A, 24, 30, UP, EXIT_STAIRS, &bank_floor4},

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
  { MAP_A, 27, 26, UP, str_floor3_choose_wisely},

  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

static void on_lever_pull(const Lever *lever) {
  (void)lever;
  play_sound(sfx_monster_critical);
  open_door(DOOR_1);
  map_textbox(str_floor2_door_opens);
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
  { LEVER_1, MAP_A,  1, 30, true, false, on_lever_pull },
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
  { DOOR_1, MAP_A, 17, 6, DOOR_NORMAL, false, false },
  { DOOR_2, MAP_A, 26, 14, DOOR_STAIRS_DOWN, false, false },
  { DOOR_3, MAP_A, 4, 12, DOOR_NEXT_LEVEL, false, false },
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
  { SCONCE_STATIC, MAP_A, 13, 6, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 25, 6, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 3, 12, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 5, 12, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 6, 29, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 18, 29, true, FLAME_BLUE },

  // Boss Door sconces
  { SCONCE_1, MAP_A, 24, 13, false, FLAME_RED },
  { SCONCE_2, MAP_A, 25, 13, false, FLAME_RED },
  { SCONCE_3, MAP_A, 27, 13, false, FLAME_RED },
  { SCONCE_4, MAP_A, 28, 13, false, FLAME_RED },

  // Skull "push button" sconces
  { SCONCE_5, MAP_A, 3, 24, false, FLAME_RED },
  { SCONCE_6, MAP_A, 9, 24, false, FLAME_RED },

  { SCONCE_7, MAP_A, 22, 1, false, FLAME_RED },
  { SCONCE_8, MAP_A, 28, 1, false, FLAME_RED },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------

static void on_boss_victory(void) BANKED {
  open_door(DOOR_3);
  set_npc_invisible(NPC_1);
  play_sound(sfx_big_door_open);
}

static void on_elite_victory(void) BANKED {
  set_npc_invisible(NPC_2);
  grant_ability(ABILITY_2);
  play_sound(sfx_big_powerup);
  map_textbox(get_grant_message(ABILITY_2));
}

static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  gelatinous_cube_generator(monster, 24, B_TIER);
  monster->id = 'A';
  set_on_victory(on_boss_victory);
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  zombie_generator(monster, 21, B_TIER);
  monster->id = 'A';
  set_on_victory(on_elite_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    if (player.level < 24) {
      map_textbox(str_floor3_boss_not_yet);
      return true;
    }
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor3_boss, on_boss_encouter);
    return true;
  case NPC_2:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor3_brains, on_elite_encouter);
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
  { NPC_1, MAP_A, 4, 14, MONSTER_GELATINOUS_CUBE, S_TIER, on_npc_action },
  { NPC_2, MAP_A, 3, 2, MONSTER_ZOMBIE, A_TIER, on_npc_action },
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
Kobold
*/

// Max Level: 19
static const EncounterTable encounters_low[] = {
  {
    ODDS_10P, MONSTER_LAYOUT_1,
    MONSTER_BUGBEAR, 18, B_TIER,
  },
  {
    ODDS_20P, MONSTER_LAYOUT_1,
    MONSTER_ZOMBIE, 18, B_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_3S,
    MONSTER_KOBOLD, 17, B_TIER,
    MONSTER_KOBOLD, 19, B_TIER,
    MONSTER_KOBOLD, 18, B_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 19, C_TIER,
    MONSTER_BUGBEAR, 18, C_TIER,
  },
  { END }
};

// Max Level: 23
static const EncounterTable encounters_high[] = {
  {
    ODDS_20P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 21, A_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_1M_2S,
    MONSTER_BUGBEAR, 20, B_TIER,
    MONSTER_GOBLIN, 23, C_TIER,
    MONSTER_GOBLIN, 21, C_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 22, C_TIER,
    MONSTER_ZOMBIE, 22, C_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_3S,
    MONSTER_KOBOLD, 21, C_TIER,
    MONSTER_GOBLIN, 20, B_TIER,
    MONSTER_KOBOLD, 19, C_TIER,
  },
  { END }
};

bool special_enc_1 = false;
bool special_enc_2 = false;
bool special_enc_3 = false;
bool special_enc_4 = false;

static bool on_init(void) {
  special_enc_1 = false;
  special_enc_2 = false;
  special_enc_3 = false;
  special_enc_4 = false;
  config_random_encounter(4, 1, 1, true);
  return false;
}

static void goblin_defender_encounter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  goblin_generator(monster, 19, B_TIER);
  monster->id = 'A';
  start_battle();
}

static bool on_special(void) {
  if (player_at(3, 26) && !special_enc_1) {
    goblin_defender_encounter();
    special_enc_1 = true;
    return true;
  }

  if (player_at(9, 26) && !special_enc_2) {
    goblin_defender_encounter();
    special_enc_2 = true;
    return true;
  }

  if (player_at(22, 3) && !special_enc_3) {
    goblin_defender_encounter();
    special_enc_3 = true;
    return true;
  }

  if (player_at(28, 3) && !special_enc_4) {
    goblin_defender_encounter();
    special_enc_4 = true;
    return true;
  }

  return false;
}

static bool on_move(void) {
  if (check_random_encounter()) {
    if (player.level < 21)
      generate_encounter(encounters_low);
    else
      generate_encounter(encounters_high);
    start_battle();
    return true;
  }
  return false;
}

static void check_sconces(void) {
  if (
    is_sconce_lit(SCONCE_1) &&
    is_sconce_lit(SCONCE_2) &&
    is_sconce_lit(SCONCE_3) &&
    is_sconce_lit(SCONCE_4)
  ) {
    play_sound(sfx_monster_critical);
    open_door(DOOR_2);
    map_textbox(str_floor2_door_opens);
  } else {
    play_sound(sfx_light_fire);
  }
}

static bool on_action(void) {
  if (player_at_facing(3, 26, UP) && !is_sconce_lit(SCONCE_1)) {
    light_sconce(SCONCE_1, FLAME_RED);
    light_sconce(SCONCE_5, FLAME_RED);
    check_sconces();
    return true;
  }

  if (player_at_facing(9, 26, UP) && !is_sconce_lit(SCONCE_2)) {
    light_sconce(SCONCE_2, FLAME_RED);
    light_sconce(SCONCE_6, FLAME_RED);
    check_sconces();
    return true;
  }

  if (player_at_facing(22, 3, UP) && !is_sconce_lit(SCONCE_3)) {
    light_sconce(SCONCE_3, FLAME_RED);
    light_sconce(SCONCE_7, FLAME_RED);
    check_sconces();
    return true;
  }

  if (player_at_facing(28, 3, UP) && !is_sconce_lit(SCONCE_4)) {
    light_sconce(SCONCE_4, FLAME_RED);
    light_sconce(SCONCE_8, FLAME_RED);
    check_sconces();
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

static const palette_color_t palettes[] = {
  // Palette 1 - Core background tiles
  RGB8(197, 244, 135),
  RGB8(85, 166, 57),
  RGB8(74, 45, 100),
  RGB8(37, 20, 0),
  // Palette 2 - Treasure chests
  RGB8(224, 173, 18),
  RGB8(85, 166, 57),
  RGB8(74, 45, 100),
  RGB8(37, 20, 0),
  // Palette 3 - Treasure chest "special"
  RGB8(86, 221, 146),
  RGB8(85, 166, 57),
  RGB8(74, 45, 100),
  RGB8(37, 20, 0),
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
const Floor floor3 = {
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
