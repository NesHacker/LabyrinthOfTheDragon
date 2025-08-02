#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 1
#define DEFAULT_X 12
#define DEFAULT_Y 16

bool special_encounter = true;

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_17, floor_one_v2, 32, 32 },
  { END }
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
    MAP_A, 6, 2, false, false,
    NULL, NULL,
    chest_add_torch,
  },
  {
    CHEST_2,
    MAP_A, 21, 13, false, false,
    NULL, NULL,
    chest_add_magic_key,
  },
  {
    CHEST_3,
    MAP_A, 21, 10, false, false,
    str_chest_item_1pots,
    chest_item_1pot,
  },
  {
    CHEST_4,
    MAP_A, 2, 2, true, false,
    str_chest_item_2pots,
    chest_item_2pots,
  },
  {
    CHEST_5,
    MAP_A, 29, 21, false, false,
    str_chest_item_1eth,
    chest_item_1eth,
  },

  { END },
};

//------------------------------------------------------------------------------
// Exits
//------------------------------------------------------------------------------

static const Exit exits[] = {
  // Key door
  { MAP_A, 4, 8, MAP_A, 4, 5, UP },
  { MAP_A, 4, 5, MAP_A, 4, 8, DOWN },

  // Boss Door
  { MAP_A, 12, 12, MAP_A, 12, 9, UP },
  { MAP_A, 12, 9, MAP_A, 12, 12, DOWN },

  // Main chamber to 2nd floor
  { MAP_A, 16, 13, MAP_A, 3, 24, DOWN },
  { MAP_A, 3, 24, MAP_A, 16, 13, DOWN },

  // Secret Door to Magic Key Chamber
  { MAP_A, 6, 29, MAP_A, 19, 18, DOWN },
  { MAP_A, 19, 18, MAP_A, 6, 29, DOWN },

  // Door to elite lair
  { MAP_A, 9, 23, MAP_A, 21, 28, UP },
  { MAP_A, 21, 28, MAP_A, 9, 23, DOWN },

  // To Floor 2
  { MAP_A, 12, 3, MAP_A, 12, 16, UP, EXIT_STAIRS, &bank_floor1 },

  {END},
};

//------------------------------------------------------------------------------
// Signs
//------------------------------------------------------------------------------

static const Sign signs[] = {
  // Monster fear fire (wall sign)
  { MAP_A, 4, 2, UP, str_floor1_sign_monster_no_fire },
  // Empty chest on floor 2
  { MAP_A, 2, 30, DOWN, str_floor1_sign_empty_chest },
  // The tunnel has caved in!
  { MAP_A, 12, 17, DOWN, str_floor1_sign_tunnel_cave_in },
  // Skull seemingly out of place
  { MAP_A, 10, 23, UP, str_floor1_sign_skull_out_of_place },
  // A powerful foe once lived here.
  { MAP_A, 21, 25, UP, str_floor1_sign_missing_elite },
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
  // Treasure room door
  { DOOR_1, MAP_A, 4, 8, DOOR_NORMAL, true, false },
  // Boss Room Door
  { DOOR_2, MAP_A, 12, 12, DOOR_NORMAL, false, false },
  // Next Level Door
  { DOOR_3, MAP_A, 12, 3, DOOR_NEXT_LEVEL, false, false },
  // Door to the elite lair
  { DOOR_4, MAP_A, 9, 23, DOOR_STAIRS_UP, false, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

static void on_lit(const Sconce* sconce) {
  switch (sconce->id) {
  case SCONCE_1:
    set_chest_unlocked(CHEST_4);
    map_textbox(str_floor_test_chest_click);
    break;
  case SCONCE_2:
  case SCONCE_3:
    if (is_sconce_lit(SCONCE_2) && is_sconce_lit(SCONCE_3))
      open_door(DOOR_2);
    break;
  case SCONCE_4:
    open_door(DOOR_4);
    break;
  }
}

static const Sconce sconces[] = {
  // Treasure Room
  { SCONCE_STATIC, MAP_A, 5, 2, true, FLAME_RED },
  { SCONCE_1, MAP_A, 3, 2, false, FLAME_NONE, on_lit },

  // Main cooridor
  { SCONCE_2, MAP_A, 11, 12, false, FLAME_NONE, on_lit },
  { SCONCE_3, MAP_A, 13, 12, false, FLAME_NONE, on_lit },
  { SCONCE_STATIC, MAP_A, 6, 13, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 8, 13, true, FLAME_RED },

  // Second Floor
  { SCONCE_STATIC, MAP_A, 9, 29, true, FLAME_RED },
  { SCONCE_4, MAP_A, 8, 23, false, FLAME_NONE, on_lit },

  // Boss Room
  { SCONCE_STATIC, MAP_A, 11, 3, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 13, 3, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 20, 7, true, FLAME_RED },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------

static bool boss_cleanup(void) {
  open_door(DOOR_3);
  set_npc_invisible(NPC_1);
  play_sound(sfx_big_door_open);
  return true;
}

static void boss_victory(void) BANKED {
  map_textbox_with_action(str_floor1_boss_defeated, boss_cleanup);
}

static bool boss_encounter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  goblin_generator(monster, 10, S_TIER);
  monster->id = 'A';
  set_on_victory(boss_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  if (npc->id != NPC_1)
    return false;

  if (player.level < 10) {
    map_textbox(str_maps_boss_not_yet);
    return true;
  }

  play_sound(sfx_monster_attack1);
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
    B_TIER,           // Tier for the monster (determines palette)
    action_callback,  // Action callback to execute when the player interacts
  }
  */
  { NPC_1, MAP_A, 12, 5, MONSTER_GOBLIN, S_TIER, on_npc_action },

  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

static const EncounterTable encounter_lv5[] = {
  {
    ODDS_10P, MONSTER_LAYOUT_1,
    MONSTER_KOBOLD, 6, B_TIER,
  },
  {
    ODDS_20P, MONSTER_LAYOUT_1,
    MONSTER_GOBLIN, 5, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_KOBOLD, 5, C_TIER,
    MONSTER_KOBOLD, 6, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_GOBLIN, 5, C_TIER,
    MONSTER_KOBOLD, 5, C_TIER,
  },
  { END }
};

static const EncounterTable encounter_lv9[] = {
  {
    ODDS_20P, MONSTER_LAYOUT_1,
    MONSTER_GOBLIN, 9, B_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_3S,
    MONSTER_KOBOLD, 8, C_TIER,
    MONSTER_KOBOLD, 7, B_TIER,
    MONSTER_KOBOLD, 8, C_TIER,
  },
  {
    ODDS_25P, MONSTER_LAYOUT_3S,
    MONSTER_GOBLIN, 9, C_TIER,
    MONSTER_KOBOLD, 7, C_TIER,
    MONSTER_KOBOLD, 7, C_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 9, C_TIER,
    MONSTER_KOBOLD, 5, C_TIER,
  },
  { END }
};

static bool on_init(void) {
  config_random_encounter(7, 1, 1, true);
  return false;
}

static bool on_special(void) {
  // Special hidden "mini-boss"
  if (
    player_at(29, 22) &&
    special_encounter &&
    !disable_encounters
  ) {
    special_encounter = false;
    Monster *monster = encounter.monsters;
    reset_encounter(MONSTER_LAYOUT_1);
    kobold_generator(monster, 10, A_TIER);
    monster->id = 'A';
    start_battle();
    return true;
  }

  return false;
}

static bool on_move(void) {
  if (!check_random_encounter())
    return false;

  if (player.level < 9)
    generate_encounter(encounter_lv5);
  else
    generate_encounter(encounter_lv9);

  start_battle();
  return true;
}

static bool on_action(void) {
  return false;
}

static void on_load(void) {
  special_encounter = true;
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
  // Palette 2 - Item chests
  RGB8(192, 138, 40),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 3 - Special Chests
  RGB8(195, 222, 180),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
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
  on_action,
  on_load,
};

