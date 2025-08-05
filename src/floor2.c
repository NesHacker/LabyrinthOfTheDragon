#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 10
#define DEFAULT_Y 13

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_17, floor_two_v2, 24, 24 },
  { MAP_B, BANK_17, floor_two_v2_b, 32, 16 },
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
    MAP_A, 16, 5, false, false,
    str_chest_item_2pots,
    chest_item_2pots,
  },
  {
    CHEST_2,
    MAP_A, 18, 5, false, false,
    str_chest_item_1remedy,
    chest_item_1remedy,
  },
  {
    CHEST_3,
    MAP_B, 23, 5, false, false,
    NULL, NULL,
    chest_add_magic_key
  },
  {
    CHEST_4,
    MAP_B, 25, 5, false, false,
    NULL, NULL,
    chest_add_magic_key
  },
  {
    CHEST_5,
    MAP_B, 29, 3, false, false,
    str_chest_item_1pots,
    chest_item_1pot
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
  { MAP_A, 10, 8, MAP_A, 10, 5, UP, EXIT_STAIRS },
  { MAP_A, 10, 5, MAP_A, 10, 8, DOWN, EXIT_STAIRS },

  { MAP_A, 3, 10, MAP_A, 3, 7, UP, EXIT_STAIRS },
  { MAP_A, 3, 7, MAP_A, 3, 10, DOWN, EXIT_STAIRS },

  { MAP_A, 17, 10, MAP_A, 17, 7, UP, EXIT_STAIRS },
  { MAP_A, 17, 7, MAP_A, 17, 10, DOWN, EXIT_STAIRS },

  { MAP_A, 18, 16, MAP_B, 14, 8, DOWN, EXIT_STAIRS },
  { MAP_B, 14, 8, MAP_A, 18, 16, DOWN, EXIT_STAIRS },

  { MAP_A, 9, 19, MAP_B, 5, 11, DOWN, EXIT_STAIRS },
  { MAP_B, 5, 11, MAP_A, 9, 19, DOWN, EXIT_STAIRS },

  { MAP_A, 11, 19, MAP_B, 7, 11, DOWN, EXIT_STAIRS },
  { MAP_B, 7, 11, MAP_A, 11, 19, DOWN, EXIT_STAIRS },

  { MAP_B, 6, 2, MAP_B, 24, 8, UP, EXIT_STAIRS },
  { MAP_B, 24, 8, MAP_B, 6, 2, DOWN, EXIT_STAIRS },

  { MAP_A, 10, 1, MAP_A, 26, 17, UP, EXIT_STAIRS, &bank_floor3 },

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
  { MAP_A, 17, 4, UP, str_floor2_sign_items_room, },
  { MAP_A, 10, 19, UP, str_floor2_sign_levers },
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

void on_pulled(const Lever *lever) {
  switch (lever->id) {
  case LEVER_1:
    toggle_door(DOOR_4);
    toggle_door(DOOR_5);
    toggle_door(DOOR_6);
    toggle_door(DOOR_7);
    break;
  case LEVER_2:
  case LEVER_3:
    play_sound(sfx_light_fire);

    if (lever->id == LEVER_2) {
      light_sconce(SCONCE_1, FLAME_GREEN);
      light_sconce(SCONCE_3, FLAME_GREEN);
    } else {
      light_sconce(SCONCE_2, FLAME_GREEN);
      light_sconce(SCONCE_4, FLAME_GREEN);
    }

    if (is_lever_on(LEVER_2) && is_lever_on(LEVER_3))
      open_door(DOOR_8);
    break;
  case LEVER_4:
    map_textbox(str_floor2_door_opens);
    play_sound(sfx_monster_critical);
    open_door(DOOR_1);
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
  { LEVER_1, MAP_A, 10, 21, false, false, on_pulled },
  { LEVER_2, MAP_B, 2, 6, true, false, on_pulled },
  { LEVER_3, MAP_B, 10, 6, true, false, on_pulled },
  { LEVER_4, MAP_B, 24, 5, true, false, on_pulled },
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
  { DOOR_1, MAP_A, 10, 8, DOOR_NORMAL, false, false },
  { DOOR_2, MAP_A, 3, 10, DOOR_NORMAL, true, false },
  { DOOR_3, MAP_A, 17, 10, DOOR_NORMAL, true, false },

  { DOOR_4, MAP_A, 9, 19, DOOR_STAIRS_DOWN, false, true },
  { DOOR_5, MAP_B, 5, 11, DOOR_STAIRS_UP, false, true },

  { DOOR_6, MAP_A, 11, 19, DOOR_STAIRS_DOWN, false, false },
  { DOOR_7, MAP_B, 7, 11, DOOR_STAIRS_UP, false, false },

  { DOOR_8, MAP_B, 6, 2, DOOR_NORMAL, false, false },

  { DOOR_9, MAP_A, 10, 1, DOOR_NEXT_LEVEL, false, false },

  { END }
};

//------------------------------------------------------------------------------
// Sconces
//------------------------------------------------------------------------------

// static void on_lit(const Sconce* sconce) {
// }

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

  // Map A
  { SCONCE_STATIC, MAP_A, 3, 4, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 9, 1, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 11, 1, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 6, 11, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 14, 11, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 16, 16, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 8, 19, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 12, 19, true, FLAME_BLUE },

  // Map B
  { SCONCE_STATIC, MAP_B, 10, 2, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_B, 13, 8, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_B, 29, 1, true, FLAME_BLUE },

  // Map B - Dynamic
  { SCONCE_1, MAP_B, 5, 1, false, FLAME_GREEN },
  { SCONCE_2, MAP_B, 7, 1, false, FLAME_GREEN },
  { SCONCE_3, MAP_B, 2, 5, false, FLAME_GREEN },
  { SCONCE_4, MAP_B, 10, 5, false, FLAME_GREEN },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------

static void elite_victory(void) BANKED {
  set_npc_invisible(NPC_1);
  grant_ability(ABILITY_1);
  play_sound(sfx_big_powerup);
  map_textbox(get_grant_message(ABILITY_1));
}

static bool elite_encounter(void) {
  reset_encounter(MONSTER_LAYOUT_1);
  Monster *monster = encounter.monsters;
  bugbear_generator(monster, 20, A_TIER);
  monster->id = 'A';
  set_on_victory(elite_victory);
  start_battle();
  return true;
}

static void boss_victory(void) BANKED {
  open_door(DOOR_9);
  play_sound(sfx_big_door_open);
  set_npc_invisible(NPC_2);
}

static bool boss_encounter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  owlbear_generator(monster, 17, S_TIER);
  monster->id = 'A';
  set_on_victory(boss_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor2_elite_msg, elite_encounter);
    return true;
  case NPC_2:
    if (player.level < 17) {
      map_textbox(str_maps_boss_not_yet);
      return true;
    }
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor2_boss_msg, boss_encounter);
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

  { NPC_1, MAP_A, 3, 5, MONSTER_BUGBEAR, B_TIER, on_npc_action }, // Elite
  { NPC_2, MAP_A, 10, 3, MONSTER_OWLBEAR, S_TIER, on_npc_action }, // Boss
  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

static const EncounterTable random_enc_lv12[] = {
  // 2 B-Tier Goblins
  {
    ODDS_25P, MONSTER_LAYOUT_2,
    MONSTER_GOBLIN, 10, B_TIER,
    MONSTER_GOBLIN, 11, B_TIER,
  },
  // 1 C-Tier Zombie
  {
    ODDS_30P, MONSTER_LAYOUT_1,
    MONSTER_ZOMBIE, 12, C_TIER,
  },
  // 3 B-Tier Kobolds
  {
    ODDS_25P, MONSTER_LAYOUT_3S,
    MONSTER_KOBOLD, 9, C_TIER,
    MONSTER_KOBOLD, 9, B_TIER,
    MONSTER_KOBOLD, 9, C_TIER,
  },
  // 1 A-Tier Goblin + 2 C-Tier Kobolds
  {
    ODDS_20P, MONSTER_LAYOUT_3S,
    MONSTER_KOBOLD, 11, C_TIER,
    MONSTER_GOBLIN, 12, B_TIER,
    MONSTER_KOBOLD, 11, C_TIER,
  },
};

static const EncounterTable random_enc_lv16[] = {
  // 1 A-Tier Goblin
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_GOBLIN, 14, A_TIER,
  },
  // 2 C-Tier + 1 A-Tier Kobolds
  {
    ODDS_25P, MONSTER_LAYOUT_3S,
    MONSTER_KOBOLD, 14, C_TIER,
    MONSTER_KOBOLD, 15, A_TIER,
    MONSTER_KOBOLD, 14, C_TIER,
  },
  // 2 B-Tier Zombies
  {
    ODDS_25P, MONSTER_LAYOUT_2,
    MONSTER_ZOMBIE, 15, B_TIER,
    MONSTER_ZOMBIE, 15, B_TIER,
  },
  // 1 A-Tier Kobold
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_KOBOLD, 15, A_TIER,
  },
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
    if (player.level < 16)
      generate_encounter(random_enc_lv12);
    else
      generate_encounter(random_enc_lv16);
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
  // Palette 1 - Floor
  RGB8(194, 192, 190),
  RGB8(131, 100, 59),
  RGB8(55, 56, 0),
  RGB8(0, 29, 49),
  // Palette 2 - Walls
  RGB8(212, 222, 207),
  RGB8(163, 139, 54),
  RGB8(110, 56, 46),
  RGB8(8, 12, 50),
  // Palette 3 - Treasure Chests
  RGB8(219, 191, 34),
  RGB8(163, 139, 54),
  RGB8(110, 56, 46),
  RGB8(54, 55, 0),
  // Palette 4 - Levers
  RGB_WHITE,
  RGB8(163, 139, 54),
  RGB8(110, 56, 46),
  RGB8(54, 55, 0),
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
const Floor floor2 = {
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
