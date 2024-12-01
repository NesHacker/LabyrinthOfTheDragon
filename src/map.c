#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "hero.h"
#include "joypad.h"
#include "map.h"
#include "util.h"

Map *map_current;
MapState map_state;
uint8_t player_col;
uint8_t player_row;
uint8_t map_scroll_x = 0;
uint8_t map_scroll_y = 0;
uint8_t map_page;
MapTileAttribute map_page_attributes[256];

/**
 * Lookup table that converts map_tile ids into graphic tile ids. The graphics
 * for map tiles are laid out in a way that allows for easy editing, this makes
 * it easy to compute the position for a tile given a 6-bit tile id.
 */
const uint8_t map_tile_lookup[64] = {
  0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
  0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E,
  0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E,
  0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E,
  0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E,
  0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE,
  0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
  0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
};

// Test map data

void map0_on_init(void) {
}

void map0_on_update(void) {
}

void map0_on_draw(void) {
}

void map0_on_enter(uint8_t n) {
}

void map0_on_exit(uint8_t col, uint8_t row) {
}

void map0_on_move(uint8_t col, uint8_t row) {
}

void map0_on_special(uint8_t col, uint8_t row) {
}

const MapPage map0_pages[] = {
  { 2, map_example_0 }
};

Map map0 = {
  2,                  // Default column & row
  14,
  map0_pages,         // Map pages
  1,                  // Number of pages in the map
  // TODO Handle exits
  1,                  // Tile bank
  tile_data_dungeon,  // Tile data
  0,                  // Callback bank
  map0_on_init,
  map0_on_update,
  map0_on_draw,
  map0_on_enter,
  map0_on_exit,
  map0_on_move,
  map0_on_special
};

void load_map(Map *m) {
  map_current = m;
  load_tile_full(m->tile_bank, m->tile_data, VRAM_BG_TILES);
  player_col = m->default_start_column;
  player_row = m->default_start_row;
  map_state = MAP_STATE_WAITING;
}

void load_map_page(uint8_t page_id) {
  MapPage *page = &map_current->pages[page_id];

  uint8_t _prev_bank = _current_bank;  
  SWITCH_ROM(page->bank);
  
  uint8_t *page_data = page->data;
  uint8_t *vram_top = VRAM_BACKGROUND;
  uint8_t *vram_bottom = VRAM_BACKGROUND_XY(0, 1);
  uint8_t *attributes = map_page_attributes;

  VBK_REG = VBK_TILES;
  for (uint8_t y = 0; y < 16; y++) {
    for (uint8_t x = 0; x < 16; x++) {
      uint8_t data = *page_data++;
      uint8_t tile = map_tile_lookup[data & MAP_TILE_MASK];
      
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
      *attributes++ = data >> 5;
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

const uint16_t map_palette[] = {
  RGB_WHITE,
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(32, 0, 0)
};

void init_map(void) {
  lcd_off();
  
  init_hero();

  VBK_REG = VBK_BANK_0;
  load_tiles(1, tile_data_dungeon, VRAM_BG_TILES, 0x80);
  set_bkg_palette(0, 1, map_palette);
  
  load_map(&map0);
  load_map_page(0);

  map_scroll_x = 0;
  map_scroll_y = 0;
  move_bkg(0, 0);

  lcd_on();
}

int8_t map_dx = 0;
int8_t map_dy = 0;
uint8_t move_counter = 0;

void check_move(void) {
  if (is_down(J_UP)) {
    map_dx = 0;
    map_dy = -1;
    move_counter = 16;
    map_state = MAP_STATE_MOVING;
    hero_direction = UP;
    hero_state = HERO_WALKING;
  } else if (is_down(J_DOWN)) {
    map_dx = 0;
    map_dy = 1;
    move_counter = 16;
    map_state = MAP_STATE_MOVING;
    hero_direction = DOWN;
    hero_state = HERO_WALKING;
  } else if (is_down(J_LEFT)) {
    map_dx = -1;
    map_dy = 0;
    move_counter = 16;
    map_state = MAP_STATE_MOVING;
    hero_direction = LEFT;
    hero_state = HERO_WALKING;
  } else if (is_down(J_RIGHT)) {
    map_dx = 1;
    map_dy = 0;
    move_counter = 16;
    map_state = MAP_STATE_MOVING;
    hero_direction = RIGHT;
    hero_state = HERO_WALKING;
  } else {
    map_dx = 0;
    map_dy = 0;
    map_state = MAP_STATE_WAITING;
    hero_state = HERO_STILL;
  }
}

void update_map(void) {
  switch (map_state) {
  case MAP_STATE_WAITING:
    check_move();
    break;
  case MAP_STATE_MOVING:
    map_scroll_x += map_dx;
    map_scroll_y += map_dy;
    if (--move_counter == 0) {
      check_move();
    }
    break;
  }
  update_hero();
  move_bkg(map_scroll_x, map_scroll_y);
}

void draw_map(void) {
}
