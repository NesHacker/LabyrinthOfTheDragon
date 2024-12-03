#include <gb/gb.h>
#include <gb/cgb.h>

#include "data.h"
#include "flags.h"
#include "hero.h"
#include "map.h"
#include "palette.h"

#define FLAG_HAS_TORCH            1 << 0
#define FLAG_SCONCE_LIT           1 << 1
#define FLAG_EMPTY_CHEST_CHECKED  1 << 2

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

#define FLAME_SPRITE 32

const Map area0_maps[] = {
  {
    0,              // id
    2,              // bank
    map_example_0   // data
  },
  { 1, 2, map_example_1 }
};

const Exit area0_exits[] = {
  {
    0,      // map_id
    2, 2,   // col, row
    0,      // to_area
    1,      // to_map
    2, 3,   // to_col, to_row
    DOWN,   // heading
  },
  {
    0,      // map_id
    9, 9,   // col, row
    0,      // to_area
    1,      // to_map
    9, 10,    // to_col, to_row
    DOWN
  },
  {
    1,      // map_id
    2, 2,   // col, row
    0,      // to_area
    0,      // to_map
    2, 3,    // to_col, to_row
    DOWN
  },
  {
    1,      // map_id
    9, 9,   // col, row
    0,      // to_area
    0,      // to_map
    9, 10,    // to_col, to_row
    DOWN
  },
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
  if (update_timer(flame_timer)) {
    reset_timer(flame_timer);
    flame_state ^= 1;
    set_sprite_tile(FLAME_SPRITE, flame_state ? 0x14 : 0x04);
  }

  if (check_flags(FLAG_PAGE_TEST, FLAG_SCONCE_LIT)) {
    const uint8_t c = 7;
    const uint8_t r = 5;
    const uint8_t x = c * 16 - 4;
    const uint8_t y = r * 16 + 2;
    uint8_t sx = active_map->id == 0 ? x - map_scroll_x : 0;
    uint8_t sy = active_map->id == 0 ? y - map_scroll_y : 0;
    move_sprite(FLAME_SPRITE, sx, sy);
  }
}

void area0_on_interact(void) {
  uint8_t *vram;

  if (active_map->id == 0) {
    // Sconce
    if (map_col == 6 && map_row == 5 && hero_direction == UP) {
      if (!check_flags(FLAG_PAGE_TEST, FLAG_SCONCE_LIT)) {
        if (!check_flags(FLAG_PAGE_TEST, FLAG_HAS_TORCH)) {
          map_textbox("A sconce adorns\nthe wall\x60\x03Its flame long\nextinguished."); 
        } else {
          set_flags(FLAG_PAGE_TEST, FLAG_SCONCE_LIT);
          vram = VRAM_BACKGROUND_XY(22, 4);
          set_vram_byte(vram, 0x4E);
          set_vram_byte(vram + 1, 0x4F);
          set_vram_byte(vram + 0x20, 0x5E);
          set_vram_byte(vram + 0x20 + 1, 0x5F);
          map_textbox("Wait\x60\003What was that\nsound?");
        }
      }
    }
  
    // Boss Door
    if (map_col == 11 && map_row == 3 && hero_direction == UP) {
      map_textbox("Locked tight.\x03Whatever's behind\nthis door feels\x60\nOminous.");
    }
  
    // Torch chest
    if (
      map_col == 9 && map_row == 12 && hero_direction == DOWN &&
      !check_flags(FLAG_PAGE_TEST, FLAG_HAS_TORCH)
    ) {
      vram = VRAM_BACKGROUND_XY(18, 26);
      set_vram_byte(vram, 0x2C);
      set_vram_byte(vram + 1, 0x2D);
      set_vram_byte(vram + 0x20, 0x3C);
      set_vram_byte(vram + 0x20 + 1, 0x3D);
      set_flags(FLAG_PAGE_TEST, FLAG_HAS_TORCH);
      map_textbox("Nice!\nYou found a torch.");
    }
  }

  if (active_map->id == 1) {
    // Empty chest
    if (map_col == 3 && map_row == 10 && hero_direction == DOWN) {
      if (!check_flags(FLAG_PAGE_TEST, FLAG_EMPTY_CHEST_CHECKED)) {
        set_flags(FLAG_PAGE_TEST, FLAG_EMPTY_CHEST_CHECKED);
        map_textbox("Nothing but dust.\003Best look\nelsewhere\x60");
      } else {
        map_textbox("Yep.\nStill empty.");
      }
    }
  
    // Wall Skull
    if (map_col == 9 && map_row == 3) {
      map_textbox("This skull seems\nout of place.");
    }
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
