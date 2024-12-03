#include <gb/gb.h>
#include <gb/cgb.h>

#include "data.h"
#include "flags.h"
#include "hero.h"
#include "map.h"
#include "palette.h"

// -----------------------------------------------------------------------------
// Potentially can be factored out
// -----------------------------------------------------------------------------
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

const Map area0_maps[] = {
  // id, bank, data
  { 0, 2, map_example_0 },
  { 1, 2, map_example_1 }
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
  // TODO: Make sprite sheet loading default on areas
  VBK_REG = VBK_BANK_1;
  load_tile_page(1, tile_data_objects, VRAM_SPRITE_TILES);
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

void area0_on_interact(void) {
  uint8_t *vram;

  switch(active_map->id) {
  case 0:
    // Sconce
    if (player_at(6, 5, UP)) {
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
    if (player_at(11, 3, UP)) {
      map_textbox("Locked tight.\fWhatever's behind\nthis door feels\x60\nOminous.");
    }

    // Torch chest
    if (
      player_at(9, 12, DOWN) &&
      !check_flags(TEST_FLAGS, FLAG_HAS_TORCH)
    ) {
      vram = VRAM_BACKGROUND_XY(18, 26);
      set_vram_byte(vram, 0x2C);
      set_vram_byte(vram + 1, 0x2D);
      set_vram_byte(vram + 0x20, 0x3C);
      set_vram_byte(vram + 0x20 + 1, 0x3D);
      set_flags(TEST_FLAGS, FLAG_HAS_TORCH);
      map_textbox("Nice!\nYou found a torch.");
    }
    break;
  case 1:
    // Empty chest
    if (player_at(3, 10, DOWN)) {
      if (!check_flags(TEST_FLAGS, FLAG_EMPTY_CHEST)) {
        set_flags(TEST_FLAGS, FLAG_EMPTY_CHEST);
        map_textbox("Nothing but dust.\fBest look\nelsewhere\x60");
      } else {
        map_textbox("Yep.\nStill empty.");
      }
    }

    // Wall Skull
    if (player_at(9, 3, UP)) {
      map_textbox("This skull seems\nout of place.");
    }
    break;
  }
}

Area area0 = {
  0,                    // Id
  2, 14,                // Default column & row
  area0_maps,           // Maps
  2,                    // Number of maps in the area
  1,                    // Tile bank
  tile_data_dungeon,    // Tile data
  area0_palettes,       // Palettes (always 4 palettes / area)
  area0_exits,          // Exits
  4,                    // Number of exits in the area
  0,                    // Callback bank
  area0_on_init,        // Callbacks
  area0_on_update,
  NULL,
  area0_on_interact,
};
