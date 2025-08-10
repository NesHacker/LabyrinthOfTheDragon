#pragma bank 8

#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 8
#define DEFAULT_Y 30

bool switch_lever_1 = false;
bool switch_lever_2 = false;
bool switch_door_6 = false;
bool switch_door_8 = false;

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
    str_chest_item_3elixirs, chest_item_3elixirs,
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
  { MAP_A, 8, 26, MAP_A, 27, 9, UP, EXIT_STAIRS },
  { MAP_A, 27, 9, MAP_A, 8, 26, DOWN, EXIT_STAIRS },

  // Elite Room
  { MAP_A, 7, 26, MAP_A, 2, 19, UP, EXIT_STAIRS },
  { MAP_A, 2, 19, MAP_A, 7, 26, DOWN, EXIT_STAIRS },

  // Item Room
  { MAP_A, 9, 26, MAP_A, 10, 19, UP, EXIT_STAIRS },
  { MAP_A, 10, 19, MAP_A, 9, 26, DOWN, EXIT_STAIRS },

  // Left Wing Portal & Hole
  { MAP_A, 1, 28, MAP_A, 7, 11, HERE, EXIT_PORTAL },
  { MAP_A, 9, 6, MAP_A, 7, 29, HERE, EXIT_HOLE },

  // Right Wing Portal & Hole
  { MAP_A, 15, 28, MAP_A, 13, 11, HERE, EXIT_PORTAL },
  { MAP_A, 11, 6, MAP_A, 9, 29, HERE, EXIT_HOLE },

  // Secret Maze
  { MAP_A, 24, 7, MAP_A, 31, 29, LEFT, EXIT_STAIRS },
  { MAP_A, 31, 29, MAP_A, 24, 7, RIGHT, EXIT_STAIRS },

  // Next Floor
  { MAP_A, 27, 5, MAP_A, DEFAULT_X, DEFAULT_Y, UP, EXIT_STAIRS, &bank_floor8 },
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
  { MAP_A, 10, 26, UP, str_floor7_riddle },
  { END },
};

//------------------------------------------------------------------------------
// Levers
//------------------------------------------------------------------------------

#define START_STUCK true

uint8_t puzzle_state = 0;
const uint8_t a_lookup[8] = { 1, 4, 1, 1, 3, 6, 7, 7 };
const uint8_t b_lookup[8] = { 2, 0, 3, 5, 2, 4, 0, 7 };

static void set_eyes(uint8_t a, uint8_t b, uint8_t c) {
  set_tile_at(MAP_A, 7, 25, a ? 0x35 : 0x36);
  set_tile_at(MAP_A, 8, 24, b ? 0x35 : 0x36);
  set_tile_at(MAP_A, 9, 25, c ? 0x35 : 0x36);

  if (a)
    open_door(DOOR_3);
  else
    close_door(DOOR_3);

  if (c)
    open_door(DOOR_4);
  else
    close_door(DOOR_4);
}

static void on_pulled(const Lever *lever) {
  switch (lever->id) {
  case LEVER_1:
    puzzle_state = a_lookup[puzzle_state];
    break;
  case LEVER_2:
    puzzle_state = b_lookup[puzzle_state];
    break;
  }

  switch (puzzle_state) {
  case 0: set_eyes(0, 0, 0); break;
  case 1: set_eyes(1, 0, 0); break;
  case 2: set_eyes(0, 1, 0); break;
  case 3: set_eyes(0, 0, 1); break;
  case 4: set_eyes(1, 1, 0); break;
  case 5: set_eyes(1, 0, 1); break;
  case 6: set_eyes(0, 1, 1); break;
  case 7: set_eyes(1, 1, 1); break;
  }

  if (puzzle_state == 7) {
    open_door(DOOR_2);
    play_sound(sfx_big_door_open);
    stick_lever(LEVER_1);
    stick_lever(LEVER_2);
    set_palette_at(MAP_A, 8, 26, 4);
  } else {
    play_sound(sfx_door_unlock);
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
  { LEVER_1, MAP_A, 7, 28, false, START_STUCK, on_pulled },
  { LEVER_2, MAP_A, 9, 28, false, START_STUCK, on_pulled },
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
  { DOOR_2, MAP_A, 8, 26, DOOR_NEXT_LEVEL, false, false },  // Boss
  { DOOR_3, MAP_A, 7, 26, DOOR_STAIRS_DOWN, false, false }, // Elite
  { DOOR_4, MAP_A, 9, 26, DOOR_STAIRS_DOWN, false, false }, // Item Room
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
  if (sconce->id == SCONCE_3) {
    if (switch_door_6) {
      open_door(DOOR_6);
      play_sound(sfx_big_door_open);
    } else {
      play_sound(sfx_door_unlock);
    }
  }

  if (sconce->id == SCONCE_6) {
    if (switch_door_8) {
      open_door(DOOR_8);
      play_sound(sfx_big_door_open);
    } else {
      play_sound(sfx_door_unlock);
    }
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

  // Left wing puzzle sconces
  { SCONCE_1, MAP_A, 3, 27, false, FLAME_NONE },
  { SCONCE_2, MAP_A, 3, 9, false, FLAME_NONE },
  { SCONCE_3, MAP_A, 3, 4, false, FLAME_NONE, on_lit },

  // Right wing puzzle sconces
  { SCONCE_4, MAP_A, 13, 27, false, FLAME_NONE },
  { SCONCE_5, MAP_A, 17, 9, false, FLAME_NONE },
  { SCONCE_6, MAP_A, 17, 4, false, FLAME_NONE, on_lit },

  // Prelit sconces
  { SCONCE_STATIC, MAP_A, 6, 26, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 22, 27, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 1, 16, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 3, 16, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 9, 16, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 11, 16, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 2, 0, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 8, 4, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 12, 4, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 18, 0, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 26, 5, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 28, 5, true, FLAME_GREEN },
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
      map_textbox(str_floor7_boss_not_yet);
      return true;
    }
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor7_boss, on_boss_encouter);
    return true;
  case NPC_2:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor7_elite_attack, on_elite_encouter);
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
// Palette Colors
//------------------------------------------------------------------------------

static const palette_color_t palettes[] = {
  // Palette 1 - Core background tiles
  RGB8(80, 120, 70),
  RGB8(0, 50, 70),
  RGB8(24, 0, 40),
  RGB8(24, 0, 0),
  // Palette 2 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(0, 50, 70),
  RGB8(24, 0, 40),
  RGB8(24, 0, 0),
  // Palette 3
  RGB_WHITE,
  RGB8(0, 50, 70),
  RGB8(24, 0, 40),
  RGB8(24, 0, 0),
  // Palette 4
  RGB8(40, 0, 0),
  RGB8(0, 50, 70),
  RGB8(24, 0, 40),
  RGB8(24, 0, 0),
  // Palette 5
  RGB8(20, 180, 20),
  RGB8(0, 50, 70),
  RGB8(24, 0, 40),
  RGB8(24, 0, 0),
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
    ODDS_15P, MONSTER_LAYOUT_1,
    MONSTER_MINDFLAYER, 46, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_2,
    MONSTER_DISPLACER_BEAST, 47, C_TIER,
    MONSTER_DISPLACER_BEAST, 47, C_TIER,
  },
  {
    ODDS_15P, MONSTER_LAYOUT_2,
    MONSTER_BUGBEAR, 46, B_TIER,
    MONSTER_GOBLIN, 46, C_TIER,
  },
  {
    ODDS_35P, MONSTER_LAYOUT_3S,
    MONSTER_ZOMBIE, 44, C_TIER,
    MONSTER_ZOMBIE, 44, C_TIER,
    MONSTER_ZOMBIE, 44, C_TIER,
  },
  { END }
};

static const EncounterTable encounters_high[] = {
  {
    ODDS_25P, MONSTER_LAYOUT_1,
    MONSTER_GELATINOUS_CUBE, 45, C_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_1,
    MONSTER_WILL_O_WISP, 49, B_TIER,
  },
  {
    ODDS_30P, MONSTER_LAYOUT_2,
    MONSTER_GELATINOUS_CUBE, 48, C_TIER,
    MONSTER_GELATINOUS_CUBE, 48, C_TIER,
  },
  {
    ODDS_15P, MONSTER_LAYOUT_1,
    MONSTER_DEATHKNIGHT, 47, A_TIER,
  },
  { END }
};

static bool on_init(void) {
  switch_lever_1 = false;
  switch_lever_2 = false;
  switch_door_6 = false;
  switch_door_8 = false;
  config_random_encounter(4, 1, 1, true);
  init_teleporter_animation(2, palettes);
  return false;
}

static bool on_special(void) {
  // Cracked floor holes
  if (player_at(2, 6)) {
    set_tile_at(MAP_A, 2, 6, 0xEC);
    teleport(MAP_A, 2, 28, HERE, EXIT_HOLE);
    return true;
  }

  if (player_at(18, 6)) {
    set_tile_at(MAP_A, 18, 6, 0xEC);
    teleport(MAP_A, 14, 28, HERE, EXIT_HOLE);
    return true;
  }

  // Lever Unlock Switches
  if (player_at(7, 5) && !switch_lever_1) {
    switch_lever_1 = true;
    play_sound(sfx_door_unlock);
    unstick_lever(LEVER_1);
    set_tile_at(MAP_A, 7, 5, 0xF4);
    set_palette_at(MAP_A, 7, 4, 4);
    set_palette_at(MAP_A, 7, 28, 4);
    return true;
  }

  if (player_at(13, 5) && !switch_lever_2) {
    switch_lever_2 = true;
    play_sound(sfx_door_unlock);
    unstick_lever(LEVER_2);
    set_tile_at(MAP_A, 13, 5, 0xF4);
    set_palette_at(MAP_A, 13, 4, 4);
    set_palette_at(MAP_A, 9, 28, 4);
    return true;
  }

  // Door unlock switches
  if (player_at(4, 6) && !switch_door_6) {
    switch_door_6 = true;
    if (is_sconce_lit(SCONCE_3)) {
      open_door(DOOR_6);
      play_sound(sfx_big_door_open);
    } else {
      play_sound(sfx_door_unlock);
    }
    set_tile_at(MAP_A, 4, 6, 0xF4);
    set_palette_at(MAP_A, 4, 5, 4);
    return true;
  }

  if (player_at(16, 6) && !switch_door_8) {
    switch_door_8 = true;
    if (is_sconce_lit(SCONCE_6)) {
      open_door(DOOR_8);
      play_sound(sfx_big_door_open);
    } else {
      play_sound(sfx_door_unlock);
    }
    set_tile_at(MAP_A, 16, 6, 0xF4);
    set_palette_at(MAP_A, 16, 5, 4);
    return true;
  }

  return false;
}

static bool on_move(void) {
  if (check_random_encounter()) {
    if (player.level < 50)
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
  NULL,
  on_draw
};
