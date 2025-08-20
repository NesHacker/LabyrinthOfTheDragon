#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 8
#define DEFAULT_Y 7

//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_17, floor_six_a, 32, 32 },
  { MAP_B, BANK_17, floor_six_b, 16, 8 },
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
  // Puzzle floors
  { CHEST_1, MAP_A, 12, 16, false, false, str_chest_item_1elixir, chest_item_1elixir },
  { CHEST_2, MAP_A, 7, 25, false, false, NULL, NULL, chest_add_magic_key },
  { CHEST_3, MAP_A, 20, 25, false, false, NULL, NULL, chest_add_magic_key },

  // Treasure Room
  { CHEST_4, MAP_A, 26, 4, true, true, str_chest_item_3potions, chest_item_3potions },
  { CHEST_5, MAP_A, 27, 4, true, true, str_chest_item_3regen, chest_item_3regen},
  { CHEST_6, MAP_A, 28, 4, true, true, str_chest_item_3ethers, chest_item_3ethers },

  // Secret Boss Room
  { CHEST_7, MAP_B, 11, 3, false, false, str_chest_item_1pots, chest_item_1pot },
  { CHEST_8, MAP_B, 13, 3, false, false, str_chest_item_1eths, chest_item_1eth },

  // Treasure Room Chests
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

  // Boss Room Door
  { MAP_A, 8, 1, MAP_B, 3, 6, UP, EXIT_STAIRS },
  { MAP_B, 3, 6, MAP_A, 8, 1, DOWN, EXIT_STAIRS },

  // Elite Door
  { MAP_A, 2, 18, MAP_A, 20, 9, UP, EXIT_STAIRS },
  { MAP_A, 20, 9, MAP_A, 2, 18, DOWN, EXIT_STAIRS },

  // Item Room Door
  { MAP_A, 12, 19, MAP_A, 30, 10, UP, EXIT_STAIRS },
  { MAP_A, 30, 10, MAP_A, 12, 19, DOWN, EXIT_STAIRS },

  // 2nd Floor Holes
  { MAP_A, 4, 20, MAP_A, 5, 4, HERE, EXIT_HOLE },
  { MAP_A, 7, 18, MAP_A, 8, 2, HERE, EXIT_HOLE },
  { MAP_A, 10, 20, MAP_A, 11, 4, HERE, EXIT_HOLE },
  { MAP_A, 5, 23, MAP_A, 6, 7, HERE, EXIT_HOLE },

  // 3rd Floor Holes
  { MAP_A, 21, 21, MAP_A, 5, 16, HERE, EXIT_HOLE },
  { MAP_A, 22, 21, MAP_A, 7, 3, HERE, EXIT_HOLE },
  { MAP_A, 21, 22, MAP_A, 6, 4, HERE, EXIT_HOLE },

  { MAP_A, 23, 25, MAP_A, 7, 20, HERE, EXIT_HOLE },
  { MAP_A, 24, 24, MAP_A, 10, 4, HERE, EXIT_HOLE },
  { MAP_A, 22, 25, MAP_A, 8, 5, HERE, EXIT_HOLE },

  // Next floor
  { MAP_B, 3, 2, MAP_A, 8, 30, UP, EXIT_STAIRS, &bank_floor7},

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

uint8_t active_portal = 0;

static void pull_routing_lever(const Lever *lever) {
  (void)lever;

  const uint8_t a = is_lever_on(LEVER_1) ? 1 : 0;
  const uint8_t b = is_lever_on(LEVER_2) ? 2 : 0;

  active_portal = a + b;

  set_palette_at(MAP_A, 1, 2, 0);
  set_palette_at(MAP_A, 3, 2, 0);
  set_palette_at(MAP_A, 13, 2, 0);
  set_palette_at(MAP_A, 15, 2, 0);

  switch (active_portal) {
  case 0:
    set_palette_at(MAP_A, 1, 2, 2);
    break;
  case 1:
    set_palette_at(MAP_A, 3, 2, 2);
    break;
  case 2:
    set_palette_at(MAP_A, 13, 2, 2);
    break;
  case 3:
    set_palette_at(MAP_A, 15, 2, 2);
    break;
  }

  play_sound(sfx_door_unlock);
}

static void on_pulled(const Lever *lever) {
  switch (lever->id) {
  case (LEVER_3):
    open_door(DOOR_3);
    play_sound(sfx_big_door_open);
    stick_lever(LEVER_3);
    break;
  case (LEVER_4):
    open_door(DOOR_4);
    play_sound(sfx_big_door_open);
    stick_lever(LEVER_4);
    break;
  default:
    pull_routing_lever(lever);
    return;
  }

  if (lever->id == LEVER_3 || lever->id == LEVER_4) {
    if (is_door_open(DOOR_3) && is_door_open(DOOR_4)) {
      open_door(DOOR_1);
      map_textbox(str_floor2_door_opens);
    }
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
  { LEVER_1, MAP_A, 7, 4, false, false, on_pulled },
  { LEVER_2, MAP_A, 9, 4, false, false, on_pulled },
  { LEVER_3, MAP_A, 3, 15, false, false, on_pulled },
  { LEVER_4, MAP_A, 12, 24, false, false, on_pulled },
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
  // Boss room door
  { DOOR_1, MAP_A, 8, 1, DOOR_NORMAL, false, false },

  // Next level door
  { DOOR_2, MAP_B, 3, 2, DOOR_NEXT_LEVEL, false, false },

  // Elite Room Door
  { DOOR_3, MAP_A, 2, 18, DOOR_NORMAL, false, false },

  // Item Room Door
  { DOOR_4, MAP_A, 12, 19, DOOR_NORMAL, false, false },
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
  { SCONCE_STATIC, MAP_A, 0, 19, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 5, 12, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 14, 19, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 8, 21, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 19, 25, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 22, 17, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 24, 3, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 28, 8, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 4, 5, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 12, 5, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_B, 2, 2, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_B, 4, 2, true, FLAME_BLUE },
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
  grant_ability(ABILITY_5);
  play_sound(sfx_big_powerup);
  map_textbox(get_grant_message(ABILITY_5));
}
static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  mindflayer_generator(monster, 45, A_TIER);
  monster->id = 'A';
  set_on_victory(on_boss_victory);
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  will_o_wisp_generator(monster, 43, B_TIER);
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
  { NPC_1, MAP_B, 3, 4, MONSTER_MINDFLAYER, S_TIER, on_npc_action }, // Boss
  { NPC_2, MAP_A, 23, 4, MONSTER_WILL_O_WISP, A_TIER, on_npc_action }, // Elite
  { END }
};

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

static const palette_color_t palettes[] = {
  // Palette 1 - Core background tiles
  RGB8(100, 100, 0),
  RGB8(30, 45, 30),
  RGB8(40, 60, 40),
  RGB8(0, 0, 24),
  // Palette 2 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(30, 45, 30),
  RGB8(40, 60, 40),
  RGB8(0, 0, 24),
  // Palette 3 - Active portal
  RGB8(220, 0, 220),
  RGB8(30, 45, 30),
  RGB8(40, 60, 40),
  RGB8(0, 0, 24),
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
// Scripting Callbacks
//------------------------------------------------------------------------------

static const EncounterTable encounters_low[] = {
  {
    ODDS_10P, MONSTER_LAYOUT_1,
    MONSTER_WILL_O_WISP, 39, C_TIER,
  },
  {
    ODDS_20P, MONSTER_LAYOUT_2,
    MONSTER_GELATINOUS_CUBE, 41, C_TIER,
    MONSTER_GELATINOUS_CUBE, 41, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_1,
    MONSTER_OWLBEAR, 42, B_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_BUGBEAR, 41, C_TIER,
    MONSTER_BUGBEAR, 41, C_TIER,
  },
  { END }
};

static const EncounterTable encounters_high[] = {
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_GELATINOUS_CUBE, 45, C_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_2,
    MONSTER_OWLBEAR, 43, B_TIER,
    MONSTER_OWLBEAR, 43, B_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_1,
    MONSTER_DISPLACER_BEAST, 45, C_TIER,
  },
  {
    ODDS_15P, MONSTER_LAYOUT_3S,
    MONSTER_GOBLIN, 41, C_TIER,
    MONSTER_KOBOLD, 43, B_TIER,
    MONSTER_GOBLIN, 41, C_TIER,
  },
  { END }
};

static bool on_init(void) {
  active_portal = 0;
  init_teleporter_animation(2, palettes);
  config_random_encounter(4, 1, 1, true);
  return false;
}

static bool on_special(void) {
  switch (active_portal) {
  case 0:
    if (player_at(1, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 2, 21, UP, EXIT_PORTAL);
    }
    break;
  case 1:
    if (player_at(3, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 26, 21, UP, EXIT_PORTAL);
    }
    break;
  case 2:
    if (player_at(13, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 22, 29, LEFT, EXIT_PORTAL);
    }
    break;
  case 3:
    if (player_at(15, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 12, 21, UP, EXIT_PORTAL);
    }
    break;
  }
  return false;
}

static bool on_move(void) {
  if (check_random_encounter()) {
    if (player.level < 43)
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

static void on_draw(void) {
  animate_teleporter_colors(2);
}

//------------------------------------------------------------------------------
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor6 = {
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
  NULL,
  on_draw,
};
