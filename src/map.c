#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "joypad.h"
#include "util.h"

#define MAP_ATTR_WALL 0
#define MAP_ATTR_GROUND_SAFE 1
#define MAP_ATTR_GROUND_HOSTILE 2
#define MAP_ATTR_EXIT 3
#define MAP_ATTR_CUSTOM_1 4
#define MAP_ATTR_CUSTOM_2 5
#define MAP_ATTR_CUSTOM_3 6
#define MAP_ATTR_CUSTOM_4 7

uint8_t map_tile_type[256];

const uint8_t map_tile_lookup[32] = {
  0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
  0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E,
  0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E,
  0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E,
};

void load_map(uint8_t bank, uint8_t *map_tiles) {
  /*
    Map data is formatted as 16x16 = 256 bytes of data with each byte
    representing a 16x16 pixel region of a screen.

      [YYYTTTTT]
         ^    ^
         |    +- 5-bit tile id (0 - 31) that points to a 16x16 tile region
         |       in the first page of the tileset for the "background"
         +------ 3-bit tile type identifier (wall, friendly, hostile, door,
                 custom_1, custom_2, custom_3, custom_4).
  */

  uint8_t _prev_bank = _current_bank;  
  SWITCH_ROM(bank);

  uint8_t *vram_top = VRAM_BACKGROUND;
  uint8_t *vram_bottom = VRAM_BACKGROUND_XY(0, 1);
  uint8_t *tile_type = map_tile_type;

  VBK_REG = VBK_TILES;
  for (uint8_t y = 0; y < 16; y++) {
    for (uint8_t x = 0; x < 16; x++) {
      uint8_t data = *map_tiles++;
      
      uint8_t tile = map_tile_lookup[(data & 0x1F)];
      
      if (tile == 0) {
        *vram_top++ = 0;
        *vram_top++ = 0;
        *vram_bottom++ = 0;
        *vram_bottom++ = 0;
      } else {
        *vram_top++ = tile;
        *vram_top++ = tile + 1;
        *vram_bottom++ = tile + 16;
        *vram_bottom++ = tile + 16 + 1;
      }

      // Load the tile type into main memory
      *tile_type++ = (data & 0xE0) >> 5;
    }
    vram_top += 32;
    vram_bottom += 32;
  }

  // Currently map data doesn't include attribute information, if we want
  // to add it in the future we'd need to use the (tile, attr) pairs format
  // and take this data into account.
  VBK_REG = VBK_ATTRIBUTES;
  uint8_t *vram_attr = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    *vram_attr++ = 0;
  }
  
  SWITCH_ROM(_prev_bank);
}

uint8_t sx = 0;
uint8_t sy = 0;

const uint16_t map_palette[] = {
  RGB_WHITE,
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(32, 0, 0)
};

void init_map(void) {
  lcd_off();
  
  VBK_REG = VBK_BANK_0;
  load_tiles(1, tile_data_dungeon, VRAM_BG_TILES, 0x80);
  set_bkg_palette(0, 1, map_palette);
  load_map(2, map_example_0);
  sx = 0;
  sy = 0;
  move_bkg(0, 0);
  lcd_on();
}

void update_map(void) {
  if (is_down(J_LEFT) && sx > 0) sx -= 2;
  else if (is_down(J_RIGHT) && sx < 96) sx += 2;
  else if (is_down(J_UP) && sy > 0) sy -= 2;
  else if (is_down(J_DOWN) && sy < 112) sy += 2;
  move_bkg(sx, sy);
}

void draw_map(void) {
}
