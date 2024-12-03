#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "data.h"
#include "flags.h"
#include "hero.h"
#include "joypad.h"
#include "map.h"
#include "palette.h"
#include "textbox.h"
#include "util.h"

Area *current_area;
uint8_t current_map_id;
uint8_t map_col = 0;
uint8_t map_row = 0;
MapTileAttribute map_tile_attributes[256];

/**
 * State of the world map controller.
 */
MapState map_state;

/**
 * Absolute x position of the hero on the map.
 */
uint8_t map_x = 0;

/**
 * Absolute y position of the hero on the map.
 */
uint8_t map_y = 0;

/**
 * Horizontal background scroll for the current map.
 */
uint8_t map_scroll_x = 0;

/**
 * Vertical background scroll for the current map.
 */
uint8_t map_scroll_y = 0;

/**
 * Direction the map is currently moving.
 */
Direction map_move_direction = NO_DIRECTION;

/**
 * Frame counter for the current map move.
 */
uint8_t map_move_counter = 0;

/**
 * Reference to the exit taken prior to a progressive load. Used to initialize
 * player's sprite and such after the transition.
 */
Exit *current_exit;

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
inline void set_map_xy_from_col_row(void) {
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

const Map area0_maps[] = {
  { 2, map_example_0 },
  { 2, map_example_1 }
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


palette_color_t fire_palette[4] = {
  RGB_BLACK,
  RGB8(213, 200, 89),
  RGB8(132, 88, 32),
  RGB8(72, 21, 13),
};


#define FLAG_HAS_TORCH            1 << 0
#define FLAG_SCONCE_LIT           1 << 1
#define FLAG_EMPTY_CHEST_CHECKED  1 << 2

#define FLAME_SPRITE 32

Timer flame_timer;
uint8_t flame_state = 0;

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
    move_sprite(FLAME_SPRITE, x - map_scroll_x, y - map_scroll_y);
  }
}

void area0_on_interact(void) {
  uint8_t *vram;

  if (current_map_id == 0) {
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

  if (current_map_id == 1) {
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
      map_textbox("This skull seems\x60\nOut of place.");
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
  area0_on_interact,
};

// -----------------------------------------------------------------------------
// END TEST MAP DATA
// -----------------------------------------------------------------------------

/**
 * Loads an area state and assets and initializes the world map controller.
 * @param a Area to load.
 */
void load_area(Area *a) {
  // Initialize area state
  current_area = a;
  map_col = a->default_start_column;
  map_row = a->default_start_row;
  map_state = MAP_STATE_WAITING;
  set_map_xy_from_col_row();
  update_map_positions();

  // Load tileset and palettes
  VBK_REG = VBK_BANK_0;
  load_tile_page(a->tile_bank, a->bg_tile_data, VRAM_BG_TILES);
  load_tile_page(a->tile_bank, a->bg_tile_data + 16 * 0x80, VRAM_SHARED_TILES);
  update_bg_palettes(0, 4, a->palettes);

  // Run the initializer callback if applicable
  if (a->on_init)
    a->on_init();
}

/**
 * Loads an individual map data tile into VRAM and the map attributes table.
 * Note: this is broken out into an inline to make the map loading routine
 * easier to read.
 * 
 * @param map_data Pointer to the next map data to load.
 * @param vram Pointer to the VRAM location into which the graphics should be
 *  loaded.
 * @param map_attr Pointer to the place in the map attributes table where
 *  attribute data should be placed.
 */
inline void load_map_tile(uint8_t *map_data, uint8_t *vram, uint8_t *map_attr) {
  uint8_t data = *map_data;
  uint8_t attr = *(map_data + 1);
  uint8_t tile = map_tile_lookup[data & MAP_TILE_MASK];

  // Set tilemap attributes
  VBK_REG = VBK_ATTRIBUTES;
  *vram = attr;
  *(vram + 1) = attr;
  *(vram + 0x20) = attr;
  *(vram + 0x20 + 1) = attr;

  // Set tile from data
  VBK_REG = VBK_TILES;
  *vram = tile;
  *(vram + 0x20) = tile + 16;
  *(vram + 1) = tile + 1;
  *(vram + 0x20 + 1) = tile + 16 + 1;

  // Load the tile type into main memory
  *map_attr = data >> 6;
}

/**
 * Loads a map in the current area with the given map_id.
 * @param map_id Id of the map to load.
 */
void load_map(uint8_t map_id) {
  current_map_id = map_id;
  Map *map = &current_area->maps[map_id];

  uint8_t _prev_bank = _current_bank;  
  SWITCH_ROM(map->bank);
  
  uint8_t *map_data = map->data;
  uint8_t *vram = VRAM_BACKGROUND;
  MapTileAttribute *map_attr = map_tile_attributes;

  for (uint8_t y = 0; y < 16; y++) {
    for (uint8_t x = 0; x < 16; x++) {
      load_map_tile(map_data, vram, map_attr);
      map_data += 2;
      vram += 2;
      map_attr++;
    }
    vram += 32;
  }

  SWITCH_ROM(_prev_bank);
}

/**
 * Determines if a player can move form the current square in the given
 * direction.
 * @param d Direction to check.
 * @return `true` If the player can move in the given direction.
 */
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

inline MapTileAttribute get_map_tile_attribute(void) {
  return map_tile_attributes[map_col + map_row * 16];
}

/**
 * Handle state updates when a player lands on an exit tile. Currently the area
 * structure defines a list of exits for all maps, if this function doesn't find
 * an exit at the given position in the current map then nothing happens and the
 * tile is treated as if it were simply a "ground" tile. 
 *
 * @return `true` If an exit was found and a transition has been initiated.
 */
bool handle_exit(void) {
  MapTileAttribute a = get_map_tile_attribute();
  if (a != MAP_EXIT)
    return false;

  for (uint8_t k = 0; k < current_area->num_exits; k++) {
    Exit *x = &current_area->exits[k];
    if (x->map_id != current_map_id) continue;
    if (x->col != map_col) continue;
    if (x->row != map_row) continue;
  
    current_exit = x;
    stop_map_move();
    LCDC_REG ^= LCDCF_OBJON;
    map_state = MAP_STATE_FADE_OUT;
    fade_out();

    // TODO Add player "exiting" animation based on exit type
    // TODO Handle exits in the same map
    // TODO Handle area spanning exits
    // TODO Gracefully handle map sprites

    return true;
  }
  
  return false;
}

/**
 * Updates map state while the character is moving.
 */
void update_map_move(void) {
  // Move the map one pixel in the move direction
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

  // Update map positions
  update_map_positions();
  update_hero();
  move_bkg(map_scroll_x, map_scroll_y);
  
  // Check if the move animation is complete
  if (--map_move_counter != 0)
    return;

  // Move is complete, update the row/column and execute handlers for the
  // landing tile's type.
  map_col = map_x >> 4;
  map_row = map_y >> 4;

  switch (get_map_tile_attribute()) {
  case MAP_EXIT:
    handle_exit();
    break;
  default:
    // Immediately check for d-pad input to allow continuous movement.
    check_move();
  }
}

bool check_action(void) {
  if (was_pressed(J_A) && current_area->on_action) {
    current_area->on_action();
    return true;
  }
  return false;
}

inline void map_textbox(const char *text) {
  map_state = MAP_STATE_TEXTBOX;
  open_textbox(text); 
}

void init_world_map(void) {
  lcd_off();
  init_hero();
  load_area(&area0);
  load_map(0);
  init_text_box();
  update_sprite_palettes(1, 1, fire_palette);
  lcd_on();
}

void update_world_map(void) {
  switch (map_state) {
  case MAP_STATE_WAITING:
    if (!check_action()) {
      check_move();
    }
    update_hero();
    move_bkg(map_scroll_x, map_scroll_y);
    break;
  case MAP_STATE_MOVING:
    update_map_move();
    break;
  }

  if (current_area->on_update)
    current_area->on_update();
}

void draw_world_map(void) {
  switch (map_state) {
  case MAP_STATE_TEXTBOX:
    update_textbox();
    if (textbox_state == TEXTBOX_CLOSED) {
      map_state = MAP_STATE_WAITING;
    }
    break;
  case MAP_STATE_FADE_OUT:
    if (fade_update()) {
      map_state = MAP_STATE_LOAD;
    }
    break;
  case MAP_STATE_LOAD:
    lcd_off();
    load_map(current_exit->to_map);
    map_col = current_exit->to_col;
    map_row = current_exit->to_row;
    hero_direction = current_exit->heading;
    set_map_xy_from_col_row();
    update_map_positions();
    update_hero();
    move_bkg(map_scroll_x, map_scroll_y);
    map_state = MAP_STATE_FADE_IN;
    fade_in();
    lcd_on();
    break;
  case MAP_STATE_FADE_IN:
    if (fade_update()) {
      // TODO Animate the character based on heading type
      map_state = MAP_STATE_WAITING;
      LCDC_REG ^= LCDCF_OBJON;
    }
    break;
  }
}
