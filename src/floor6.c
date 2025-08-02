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

uint8_t active_portal = 0;

static void on_pulled(const Lever *lever) {
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

static void boss_victory(void) NONBANKED {
}

static bool boss_encounter(void) {
  return false;
}

static bool on_npc_action(const NPC *npc) {
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

  { END }
};

//------------------------------------------------------------------------------
// Scripting Callbacks
//------------------------------------------------------------------------------

uint8_t portal_color_idx = 0;
Timer portal_color_timer;
#define MAX_PORTAL_COLOR_FRAMES 6

const palette_color_t portal_color[MAX_PORTAL_COLOR_FRAMES] = {
  RGB8(220, 0, 220),
  RGB8(180, 0, 240),
  RGB8(100, 120, 120),
  RGB8(20, 240, 0),
  RGB8(0, 200, 0),
  RGB8(100, 0, 100),
};

palette_color_t portal_color_palette[4] = {
  RGB8(220, 0, 220),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
};

static bool on_init(void) {
  active_portal = 0;
  init_timer(portal_color_timer, 4);
  return false;
}

static bool on_special(void) {
  switch (active_portal) {
  case 0:
    if (player_at(1, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 2, 21, UP);
    }
    break;
  case 1:
    if (player_at(3, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 22, 29, LEFT);
    }
    break;
  case 2:
    if (player_at(13, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 26, 21, UP);
    }
    break;
  case 3:
    if (player_at(15, 2)) {
      play_sound(sfx_no_no_square);
      teleport(MAP_A, 12, 21, UP);
    }
    break;
  }
  return false;
}

static bool on_move(void) {
  return false;
}

static bool on_action(void) {
  return false;
}

static void on_draw(void) {
  if (!update_timer(portal_color_timer))
    return;
  reset_timer(portal_color_timer);

  portal_color_idx++;
  if (portal_color_idx >= MAX_PORTAL_COLOR_FRAMES)
    portal_color_idx = 0;

  portal_color_palette[0] = portal_color[portal_color_idx];
  core.load_bg_palette(portal_color_palette, 2, 1);
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
  // Palette 3 - Active portal
  RGB8(220, 0, 220),
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
