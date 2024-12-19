#pragma bank 2

#include "core.h"
#include "encounter.h"
#include "map.h"

// TODO Factor me out into a general flame class
Timer flame_timer;
uint8_t flame_state = 0;

palette_color_t fire_palette[4] = {
  RGB_BLACK,
  RGB8(213, 200, 89),
  RGB8(132, 88, 32),
  RGB8(72, 21, 13),
};

// -----------------------------------------------------------------------------
// Map specific
// -----------------------------------------------------------------------------

#define FLAG_HAS_TORCH    FLAG(0)
#define FLAG_SCONCE_LIT   FLAG(1)
#define FLAG_EMPTY_CHEST  FLAG(2)

#define FLAME_SPRITE 32

#define MAP_FLOOR1 0
#define MAP_FLOOR2 1

const Map area0_maps[] = {
  // id, bank, data
  { MAP_FLOOR1, 8, map_example_0 },
  { MAP_FLOOR2, 8, map_example_1 }
};

const Exit area0_exits[] = {
  // map_id, col, row, to_area, to_map, to_col, to_row, heading, type
  { 0, 2, 2, 0, 1, 2, 3, DOWN, EXIT_STAIRS },
  { 0, 9, 9, 0, 1, 9, 10, DOWN, EXIT_STAIRS },
  { 1, 2, 2, 0, 0, 2, 3, DOWN, EXIT_STAIRS },
  { 1, 9, 9, 0, 0, 9, 10, DOWN, EXIT_STAIRS },
};

const uint16_t area0_palettes[] = {
  // Palette 0 - Core background tiles
  RGB8(190, 200, 190),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 1 - Treasure chests
  RGB8(192, 138, 40),
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(24, 0, 0),
  // Palette 2
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
  // Palette 3
  RGB_WHITE,
  RGB8(120, 120, 120),
  RGB8(60, 60, 60),
  RGB_BLACK,
};

void area0_on_init(void) {
  update_sprite_palettes(7, 1, fire_palette);
  init_timer(flame_timer, 17);
  set_sprite_tile(FLAME_SPRITE, 0x04);
  set_sprite_prop(FLAME_SPRITE, 0b00001111);
  move_sprite(FLAME_SPRITE, 0, 0);
}

void area0_on_update(void) {
  // TODO Create a flame sprite abstraction
  if (update_timer(flame_timer)) {
    reset_timer(flame_timer);
    flame_state ^= 1;
    set_sprite_tile(FLAME_SPRITE, flame_state ? 0x14 : 0x04);
  }
  if (check_flags(TEST_FLAGS, FLAG_SCONCE_LIT)) {
    if (is_map(0)) {
      const uint8_t x = 7 * 16 - 4;
      const uint8_t y = 5 * 16 + 2;
      move_sprite(FLAME_SPRITE, x - map_scroll_x, y - map_scroll_y);
    } else {
      move_sprite(FLAME_SPRITE, 0, 0);
    }
  }
}

void area0_on_action(void) {
  uint8_t *vram;

  switch(active_map->id) {
  case MAP_FLOOR1:
    // Sconce
    if (player_at_facing(6, 5, UP)) {
      if (!check_flags(TEST_FLAGS, FLAG_SCONCE_LIT)) {
        if (!check_flags(TEST_FLAGS, FLAG_HAS_TORCH)) {
          map_textbox("A sconce adorns\nthe wall\x60\fIts flame long\nextinguished.");
        } else {
          set_flags(TEST_FLAGS, FLAG_SCONCE_LIT);
          vram = VRAM_BACKGROUND_XY(22, 4);
          set_vram_byte(vram, 0x4E);
          set_vram_byte(vram + 1, 0x4F);
          set_vram_byte(vram + 0x20, 0x5E);
          set_vram_byte(vram + 0x20 + 1, 0x5F);
          map_textbox("Wait\x60\fWhat was that\nsound?");
        }
      }
    }

    // Boss Door
    if (player_at_facing(11, 3, UP)) {
      map_textbox("Locked tight.\fWhatever's behind\nthis door feels\x60\nOminous.");
    }

    break;
  case MAP_FLOOR2:
    // Empty chest
    if (player_at_facing(3, 10, DOWN)) {
      if (!check_flags(TEST_FLAGS, FLAG_EMPTY_CHEST)) {
        set_flags(TEST_FLAGS, FLAG_EMPTY_CHEST);
        map_textbox("Nothing but dust.\fBest look\nelsewhere\x60");
      } else {
        map_textbox("Yep.\nStill empty.");
      }
    }

    // Wall Skull
    if (player_at_facing(9, 3, UP)) {
      map_textbox("This skull seems\nout of place.");
    }
    break;
  }
}

#define CHEST_TORCH 0

void area0_on_chest(Chest *chest) {
  switch (chest->id) {
  case CHEST_TORCH:
    map_textbox("Nice!\nYou found a torch.");
    break;
  }
}

bool area0_on_special(void) {
  if (active_map->id == MAP_FLOOR1 && player_at(1, 9)) {
    MonsterInstance *monster = encounter.monsters;

    // reset_encounter(MONSTER_LAYOUT_2);
    // kobold_generator(monster, player.level - 20, C_TIER);
    // monster->id = 'A';
    // kobold_generator(++monster, player.level - 20, B_TIER);
    // monster->id = 'B';


    reset_encounter(MONSTER_LAYOUT_3S);
    dummy_generator(monster, player.level, DUMMY_COWARD);
    monster->id = 'Z';
    dummy_generator(++monster, player.level, DUMMY_COWARD);
    monster->id = '7';
    dummy_generator(++monster, player.level, DUMMY_COWARD);
    monster->id = 'w';


    start_battle();

    return true;
  }
  return false;
}

Chest area0_chests[] = {
  // id, map_id, col, row, flag_page, open_flag
  { CHEST_TORCH, MAP_FLOOR1, 9, 13, TEST_FLAGS, FLAG_HAS_TORCH }
};

Tileset dungeon_tileset = { 128, 12, tile_data_dungeon };

Area area0 = {
  0,                    // Id
  2, 14,                // Default column & row
  &dungeon_tileset,
  area0_palettes,       // Palettes (always 4 palettes / area)
  2, area0_maps,
  4, area0_exits,
  1, area0_chests,
  2,                    // Callback Bank & Callbacks
  area0_on_init,
  area0_on_update,
  NULL, // on_draw
  area0_on_action,
  NULL, // before_chest
  area0_on_chest,
  NULL, // on_enter
  area0_on_special, // on_special
  NULL, // on_exit
  NULL, // on_move
};
