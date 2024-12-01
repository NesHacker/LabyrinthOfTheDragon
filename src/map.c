#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "data.h"
#include "hero.h"
#include "joypad.h"
#include "map.h"
#include "util.h"

Map *map_current;
MapState map_state;
uint8_t map_scroll_x = 0;
uint8_t map_scroll_y = 0;
uint8_t map_page;
MapTileAttribute map_tile_attributes[256];

uint8_t map_x = 0;
uint8_t map_y = 0;
uint8_t map_col = 0;
uint8_t map_row = 0;
Direction map_move_direction = NO_DIRECTION;
uint8_t map_move_counter = 0;

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
  0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
  0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E,
  0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E,
  0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E,
};

/**
 * Sets the map position x & y coordinates from the current column and row.
 */
inline void set_map_xy_from_col_row() {
  map_x = map_col << 4;
  map_y = map_row << 4;
}

/**
 * Begins a map move in the given direction.
 */
inline void start_map_move(Direction d) {
  map_move_direction = d;
  map_move_counter = 16;
  map_state = MAP_STATE_MOVING;
  hero_state = HERO_WALKING;
}

/**
 * Stops a map move.
 */
inline void stop_map_move(void) {
  map_move_direction = NO_DIRECTION;
  map_state = MAP_STATE_WAITING;
  hero_state = HERO_STILL;
}

/**
 * Sets the x & y positions for the hero and the screen scroll based on the
 * current global map position.
 */
void update_map_positions(void) {
  // Update the horizontal positions
  if (map_x <= 64) {
    map_scroll_x = 0;
    hero_x = map_x;
  } else if (map_x < 160) {
    map_scroll_x = map_x - 64;
    hero_x = 64;
  } else {
    map_scroll_x = 96;
    hero_x = map_x - 96;
  }
  // Update the vertical positions
  if (map_y <= 64) {
    map_scroll_y = 0;
    hero_y = map_y;
  } else if (map_y < 176) {
    map_scroll_y = map_y - 64;
    hero_y = 64;
  } else {
    map_scroll_y = 176 - 64;
    hero_y = map_y - 112;
  }
}

// -----------------------------------------------------------------------------
// BEGIN TEST MAP DATA
// -----------------------------------------------------------------------------

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

const uint16_t map0_palettes[] = {
  // Palette 0 (default)
  RGB_WHITE,
  RGB8(100, 100, 140),
  RGB8(40, 60, 40),
  RGB8(32, 0, 0),
  // Palette 1-5
  RGB_WHITE, RGB8(120, 120, 120), RGB8(60, 60, 60), RGB8(8, 16, 0),
  RGB_WHITE, RGB8(120, 120, 120), RGB8(60, 60, 60), RGB_BLACK,
  RGB_WHITE, RGB8(120, 120, 120), RGB8(60, 60, 60), RGB_BLACK,
  RGB_WHITE, RGB8(120, 120, 120), RGB8(60, 60, 60), RGB_BLACK,
  RGB_WHITE, RGB8(120, 120, 120), RGB8(60, 60, 60), RGB_BLACK,
};

Map map0 = {
  2,                  // Default column & row
  14,
  map0_pages,         // Map pages
  1,                  // Number of pages in the map
  // TODO Handle exits
  1,                  // Tile bank
  tile_data_dungeon,  // Tile data
  map0_palettes,      // Palettes
  0,                  // Callback bank
  map0_on_init,
  map0_on_update,
  map0_on_draw,
  map0_on_enter,
  map0_on_exit,
  map0_on_move,
  map0_on_special
};

// -----------------------------------------------------------------------------
// END TEST MAP DATA
// -----------------------------------------------------------------------------

void load_map(Map *m) {
  // Load tileset and palettes
  VBK_REG = VBK_BANK_0;
  load_tile_page(m->tile_bank, m->bg_tile_data, VRAM_BG_TILES);
  load_tile_page(m->tile_bank, m->bg_tile_data + 16 * 0x80, VRAM_SHARED_TILES);
  set_bkg_palette(0, 6, m->palettes);
  
  // Initialize map state
  map_current = m;
  map_col = m->default_start_column;
  map_row = m->default_start_row;
  map_state = MAP_STATE_WAITING;
  set_map_xy_from_col_row();
  update_map_positions();
}

void load_map_page(uint8_t page_id) {
  MapPage *page = &map_current->pages[page_id];

  uint8_t _prev_bank = _current_bank;  
  SWITCH_ROM(page->bank);
  
  uint8_t *page_data = page->data;
  uint8_t *vram_top = VRAM_BACKGROUND;
  uint8_t *vram_bottom = VRAM_BACKGROUND_XY(0, 1);
  MapTileAttribute *page_attributes = map_tile_attributes;

  for (uint8_t y = 0; y < 16; y++) {
    for (uint8_t x = 0; x < 16; x++) {
      uint8_t data = *page_data++;
      uint8_t attr = *page_data++;
      uint8_t tile = map_tile_lookup[data & MAP_TILE_MASK];
      
      // Set tilemap attributes
      VBK_REG = VBK_ATTRIBUTES;
      *vram_top = attr;
      *(vram_top + 1) = attr;
      *vram_bottom = attr;
      *(vram_bottom + 1) = attr;

      // Set tile from data
      VBK_REG = VBK_TILES;
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
      *page_attributes++ = data >> 6;
    }
    vram_top += 32;
    vram_bottom += 32;
  }

  SWITCH_ROM(_prev_bank);
}

void init_map(void) {
  lcd_off();
  init_hero();
  load_map(&map0);
  load_map_page(0);
  lcd_on();
}

bool can_move(Direction d) {
  uint8_t col = map_col;
  uint8_t row = map_row;

  // Even if the hero cannot move in the direction, update the heading
  hero_direction = d;

  if (d == UP) {
    if (row == 0) return false;
    row--;
  } else if (d == DOWN) {
    if (row == 15) return false;
    row++;
  } else if (d == LEFT) {
    if (col == 0) return false;
    col--;
  } else if (d == RIGHT) {
    if (col == 15) return false;
    col++;
  }

  uint16_t k = row * 16 + col;
  MapTileAttribute t = map_tile_attributes[k];

  return t != MAP_WALL;
}

void check_move(void) {
  if (is_down(J_UP) && can_move(UP))
    start_map_move(UP);
  else if (is_down(J_DOWN) && can_move(DOWN))
    start_map_move(DOWN);
  else if (is_down(J_LEFT) && can_move(LEFT))
    start_map_move(LEFT);
  else if (is_down(J_RIGHT) && can_move(RIGHT))
    start_map_move(RIGHT);
  else
    stop_map_move();
}

void update_map_move(void) {
  switch (map_move_direction) {
  case UP:
    map_y--;
    break;
  case DOWN:
    map_y++;
    break;
  case LEFT:
    map_x--;
    break;
  case RIGHT:
    map_x++;
    break;
  }

  update_map_positions();

  if (--map_move_counter == 0) {
    map_col = map_x >> 4;
    map_row = map_y >> 4;
    check_move();
  }
}

void update_map(void) {
  switch (map_state) {
  case MAP_STATE_WAITING:
    check_move();
    break;
  case MAP_STATE_MOVING:
    update_map_move();
    break;
  }
  update_hero();
  move_bkg(map_scroll_x, map_scroll_y);
}

void draw_map(void) {
}
