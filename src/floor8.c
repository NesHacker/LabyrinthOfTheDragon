#pragma bank 8

#include "core.h"
#include "credits.h"
#include "floor.h"
#include "sound.h"

//------------------------------------------------------------------------------
// Floorwide settings
//------------------------------------------------------------------------------

#define ID 99
#define DEFAULT_X 8
#define DEFAULT_Y 29

typedef enum MiniBoss {
  MINI_BOSS_GOBLIN    = FLAG(0),
  MINI_BOSS_OWLBEAR   = FLAG(1),
  MINI_BOSS_GCUBE     = FLAG(2),
  MINI_BOSS_DBEAST    = FLAG(3),
  MINI_BOSS_DKNIGHT   = FLAG(4),
  MINI_BOSS_MFLAYER   = FLAG(5),
  MINI_BOSS_BEHOLDER  = FLAG(6),
  MINI_BOSS_ALL       = 0x7F,
} MiniBoss;

typedef enum HealingMirrorId {
  MIRROR_1 = FLAG(0),
  MIRROR_2 = FLAG(1),
  MIRROR_3 = FLAG(2),
  MIRROR_4 = FLAG(3),
  MIRROR_5 = FLAG(4),
  MIRROR_6 = FLAG(5),
} HealingMirrorId;

uint8_t mini_bosses_defeated = 0;
uint8_t current_mini_boss = 0;
uint8_t healing_mirrors_used = 0;

inline bool is_healing_mirror_used(HealingMirrorId id) {
  return healing_mirrors_used & id;
}

inline bool has_beaten(MiniBoss b) {
  return mini_bosses_defeated & b;
}

inline bool set_beaten(MiniBoss b) {
  mini_bosses_defeated |= b;
  *(debug + 0x20) = mini_bosses_defeated;
}

inline bool all_bosses_beaten(void) {
  return mini_bosses_defeated == MINI_BOSS_ALL;
}

static void on_mini_boss_victory(void) BANKED {
  set_beaten(current_mini_boss);

  switch (current_mini_boss) {
  case MINI_BOSS_GOBLIN:
    set_tile_at(MAP_A, 5, 8, 0x36);
    break;
  case MINI_BOSS_OWLBEAR:
    set_tile_at(MAP_A, 11, 8, 0x36);
    break;
  case MINI_BOSS_GCUBE:
    set_tile_at(MAP_A, 6, 8, 0x36);
    break;
  case MINI_BOSS_DBEAST:
    set_tile_at(MAP_A, 10, 8, 0x36);
    break;
  case MINI_BOSS_DKNIGHT:
    set_tile_at(MAP_A, 7, 8, 0x36);
    break;
  case MINI_BOSS_MFLAYER:
    set_tile_at(MAP_A, 9, 8, 0x36);
    break;
  case MINI_BOSS_BEHOLDER:
    set_tile_at(MAP_A, 8, 8, 0x36);
    break;
  }

  if (!is_door_open(DOOR_1) && all_bosses_beaten()) {
    open_door(DOOR_1);
    play_sound(sfx_big_door_open);
    if (current_mini_boss != MINI_BOSS_BEHOLDER)
      map_textbox(str_floor2_door_opens);
    return;
  }

  play_sound(sfx_light_fire);
}

static void mini_boss_encounter(MiniBoss b) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);

  switch (b) {
  case MINI_BOSS_GOBLIN:
    goblin_generator(monster, 45, A_TIER);
    break;
  case MINI_BOSS_OWLBEAR:
    owlbear_generator(monster, 45, A_TIER);
    break;
  case MINI_BOSS_GCUBE:
    gelatinous_cube_generator(monster, 45, A_TIER);
    break;
  case MINI_BOSS_DBEAST:
    displacer_beast_generator(monster, 45, A_TIER);
    break;
  case MINI_BOSS_DKNIGHT:
    deathknight_generator(monster, 45, A_TIER);
    break;
  case MINI_BOSS_MFLAYER:
    mindflayer_generator(monster, 45, A_TIER);
    break;
  case MINI_BOSS_BEHOLDER:
    beholder_generator(monster, 45, A_TIER);
    break;
  default:
    return;
  }

  monster->id = 'A';

  current_mini_boss = b;
  set_on_victory(on_mini_boss_victory);

  start_battle();
}


//------------------------------------------------------------------------------
// Maps
//------------------------------------------------------------------------------

static const Map maps[] = {
  // id, bank, data, width, height
  { MAP_A, BANK_16, floor_eight_data, 32, 32 },

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
    MAP_A, 2, 26,
    false, false,
    str_chest_item_3potions,
    chest_item_3potions,
  },
  {
    CHEST_2,
    MAP_A, 14, 26,
    false, false,
    str_chest_item_3ethers,
    chest_item_3ethers,
  },
  {
    CHEST_3,
    MAP_A, 3, 21,
    false, false,
    str_chest_item_1atkup_1defup,
  },
  {
    CHEST_4,
    MAP_A, 13, 21,
    false, false,
    str_chest_item_3haste,
    chest_item_3haste,
  },
  {
    CHEST_5,
    MAP_A, 4, 16,
    false, false,
    str_chest_item_3regen,
    chest_item_3regen,
  },
  {
    CHEST_6,
    MAP_A, 12, 16,
    false, false,
    str_chest_item_3elixirs,
    chest_item_3elixirs,
  },
  {
    CHEST_7,
    MAP_A, 20, 9,
    false, false,
    str_chest_item_3elixirs,
    chest_item_3elixirs,
  },
  {
    CHEST_8,
    MAP_A, 22, 9,
    false, false,
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
  { MAP_A, 8, 9, MAP_A, 8, 6, UP, EXIT_STAIRS },
  { MAP_A, 8, 6, MAP_A, 8, 9, DOWN, EXIT_STAIRS },
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
  { DOOR_1, MAP_A, 8, 9, DOOR_NORMAL, false, false },
  { DOOR_2, MAP_A, 8, 1, DOOR_NEXT_LEVEL, false, false },
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

  // Static level sconces
  { SCONCE_STATIC, MAP_A, 6, 26, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 10, 26, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 6, 21, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 10,  21, true, FLAME_GREEN },
  { SCONCE_STATIC, MAP_A, 6, 16, true, FLAME_BLUE },
  { SCONCE_STATIC, MAP_A, 10, 16, true, FLAME_BLUE },

  { SCONCE_STATIC, MAP_A, 4, 10, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 12, 10, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 21, 9, true, FLAME_RED },

  // Boss Room
  { SCONCE_STATIC, MAP_A, 7, 1, true, FLAME_RED },
  { SCONCE_STATIC, MAP_A, 9, 1, true, FLAME_RED },

  { END }
};

//------------------------------------------------------------------------------
// NPCs (IMPLS YET)
//------------------------------------------------------------------------------
static void on_elite_victory(void) BANKED {
  current_mini_boss = MINI_BOSS_BEHOLDER;
  on_mini_boss_victory();
  set_npc_invisible(NPC_2);
}

static bool on_boss_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  dragon_generator(monster, 60, A_TIER);
  monster->id = 'A';
  encounter.is_final_boss = true;
  start_battle();
  return true;
}

static bool on_elite_encouter(void) {
  Monster *monster = encounter.monsters;
  reset_encounter(MONSTER_LAYOUT_1);
  beholder_generator(monster, 55, B_TIER);
  monster->id = 'A';
  set_on_victory(on_elite_victory);
  start_battle();
  return true;
}

static bool on_npc_action(const NPC *npc) {
  switch (npc->id) {
  case NPC_1:
    play_sound(sfx_monster_attack2);
    map_textbox_with_action(str_floor8_boss, on_boss_encouter);
    return true;
  case NPC_2:
    play_sound(sfx_monster_attack1);
    map_textbox_with_action(str_floor8_elite, on_elite_encouter);
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
  { NPC_1, MAP_A, 8, 3, MONSTER_DRAGON, S_TIER, on_npc_action },
  { NPC_2, MAP_A, 8, 11, MONSTER_BEHOLDER, A_TIER, on_npc_action },
  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

static bool on_init(void) {
  mini_bosses_defeated = 0;
  return false;
}

inline bool check_mini_boss_tile(MiniBoss b, uint8_t x, uint8_t y) {
  if (player_at(x, y) && !has_beaten(b)) {
    mini_boss_encounter(b);
    return true;
  }
  return false;
}

inline bool use_healing_mirror(HealingMirrorId id) {
  healing_mirrors_used |= id;
}


bool check_healing_mirror(HealingMirrorId id, uint8_t x, uint8_t y) {
  if (!player_at_facing(x, y, UP))
    return false;

  if (is_healing_mirror_used(id)) {
    map_textbox(str_floor8_healing_mirror_none);
    return true;
  }

  full_heal_player();
  healing_mirrors_used |= id;

  map_textbox(str_floor8_healing_mirror);
  play_sound(sfx_big_powerup);
  set_palette_at(MAP_A, x, y - 1, 4);

  return true;
}

static bool on_special(void) {
  if (check_mini_boss_tile(MINI_BOSS_GOBLIN, 2, 27))
    return true;
  if (check_mini_boss_tile(MINI_BOSS_OWLBEAR, 14, 27))
    return true;
  if (check_mini_boss_tile(MINI_BOSS_GCUBE, 3, 22))
    return true;
  if (check_mini_boss_tile(MINI_BOSS_DBEAST, 13, 22))
    return true;
  if (check_mini_boss_tile(MINI_BOSS_DKNIGHT, 4, 17))
    return true;
  if (check_mini_boss_tile(MINI_BOSS_MFLAYER, 12, 17))
    return true;

  return false;
}

static bool on_move(void) {
  return false;
}

static bool on_action(void) {
  if (check_healing_mirror(MIRROR_1, 5, 27))
    return true;
  if (check_healing_mirror(MIRROR_2, 11, 27))
    return true;
  if (check_healing_mirror(MIRROR_3, 5, 22))
    return true;
  if (check_healing_mirror(MIRROR_4, 11, 22))
    return true;
  if (check_healing_mirror(MIRROR_5, 19, 10))
    return true;
  if (check_healing_mirror(MIRROR_6, 23, 10))
    return true;

  return false;
}

//------------------------------------------------------------------------------
// Palette Colors
//------------------------------------------------------------------------------

static const palette_color_t palettes[] = {
  // Palette 1 - Core background tiles
  RGB8(100, 80, 80),
  RGB8(80, 20, 20),
  RGB8(40, 0, 0),
  RGB8(24, 0, 0),
  // Palette 2 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(60, 40, 40),
  RGB8(40, 0, 0),
  RGB8(24, 0, 0),
  // Palette 3 - Floor
  RGB8(80, 80, 60),
  RGB8(60, 40, 40),
  RGB8(40, 0, 0),
  RGB8(24, 0, 0),
  // Palette 4 - Healing Mirror Full
  RGB8(00, 160, 90),
  RGB8(80, 20, 20),
  RGB8(40, 0, 0),
  RGB8(24, 0, 0),
  // Palette 5 - Heling Mirror Empty
  RGB8(40, 20, 20),
  RGB8(80, 20, 20),
  RGB8(40, 0, 0),
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
// Area Definition (shouldn't have to touch this)
//------------------------------------------------------------------------------
const Floor floor8 = {
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
