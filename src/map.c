#pragma bank 2

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "battle.h"
#include "core.h"
#include "floors.h"
#include "map.h"


Area *active_area;
Map *active_map;
Exit *active_exit;

MapTileAttribute map_tile_attributes[256];

uint8_t current_map_id;

MapState map_state;
uint8_t map_x = 0;
uint8_t map_y = 0;
uint8_t map_col = 0;
uint8_t map_row = 0;
uint8_t map_scroll_x = 0;
uint8_t map_scroll_y = 0;

Direction map_move_direction = NO_DIRECTION;
uint8_t map_move_counter = 0;

HeroState hero_state = HERO_STILL;
Direction hero_direction = DOWN;
uint8_t hero_x = 0 + 8;
uint8_t hero_y = 0 + 16;

Timer walk_timer;
uint8_t walk_frame;

extern const uint8_t map_tile_lookup[];

void init_hero(void) {
  // const uint8_t x = 8 + 16 * 4;
  // const uint8_t y = 16 + 16 * 4;

  const uint8_t offset = 0x00;
  const uint8_t frame = (walk_frame << 1) + offset;

  core.load_hero_tiles(1);
  const uint16_t hpal[4] = {
    RGB(0, 0, 0),
    RGB8(245, 213, 135),
    RGB8(167, 75, 0),
    RGB8(8, 46, 54),
  };
  core.load_sprite_palette(hpal, 0, 1);

  init_timer(walk_timer, 8);

  set_sprite_tile(0, frame);
  set_sprite_tile(1, frame + 0x01);
  set_sprite_tile(2, frame + 0x10);
  set_sprite_tile(3, frame + 0x11);

  set_sprite_prop(0, 0);
  set_sprite_prop(1, 0);
  set_sprite_prop(2, 0);
  set_sprite_prop(3, 0);

  move_sprite(0, hero_x + 8, hero_y + 16);
  move_sprite(1, hero_x + 8 + 8, hero_y + 16);
  move_sprite(2, hero_x + 8, hero_y + 8 + 16);
  move_sprite(3, hero_x + 8 + 8, hero_y + 8 + 16);
}

void update_hero(void) {
  if (hero_state == HERO_WALKING) {
    if (update_timer(walk_timer)) {
      reset_timer(walk_timer);
      walk_frame ^= 1;
    }
  } else {
    walk_frame = 0;
  }

  uint8_t offset = 0x00;
  switch (hero_direction) {
  case DOWN:
    offset = 0x00;
    break;
  case UP:
    offset = 0x04;
    break;
  case LEFT:
  case RIGHT:
    offset = 0x08;
    break;
  }

  const uint8_t frame = (walk_frame << 1) + offset;

  if (hero_direction != LEFT) {
    set_sprite_tile(0, frame);
    set_sprite_tile(1, frame + 0x01);
    set_sprite_tile(2, frame + 0x10);
    set_sprite_tile(3, frame + 0x11);
    set_sprite_prop(0, 0b00000000);
    set_sprite_prop(1, 0b00000000);
    set_sprite_prop(2, 0b00000000);
    set_sprite_prop(3, 0b00000000);
  } else {
    set_sprite_tile(1, frame);
    set_sprite_tile(0, frame + 0x01);
    set_sprite_tile(3, frame + 0x10);
    set_sprite_tile(2, frame + 0x11);
    set_sprite_prop(0, 0b00100000);
    set_sprite_prop(1, 0b00100000);
    set_sprite_prop(2, 0b00100000);
    set_sprite_prop(3, 0b00100000);
  }

  move_sprite(0, hero_x + 8, hero_y + 16);
  move_sprite(1, hero_x + 8 + 8, hero_y + 16);
  move_sprite(2, hero_x + 8, hero_y + 8 + 16);
  move_sprite(3, hero_x + 8 + 8, hero_y + 8 + 16);
}

void clear_hero(void) {
  move_sprite(0, 0, 0);
  move_sprite(1, 0, 0);
  move_sprite(2, 0, 0);
  move_sprite(3, 0, 0);
}

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

void load_area_graphics(Area *a) {
  VBK_REG = VBK_BANK_0;
  core.load_tileset(a->tileset, VRAM_BG_TILES);
  core.load_object_tiles();
  core.load_bg_palette(a->palettes, 0, 4);
}

/**
 * Loads an area state and assets and initializes the world map controller.
 * @param a Area to load.
 */
void load_area(Area *a) {
  active_area = a;
  map_col = a->default_start_column;
  map_row = a->default_start_row;
  map_state = MAP_STATE_WAITING;
  set_map_xy_from_col_row();
  update_map_positions();
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
  // TODO This is a data loader, refactor into a core data service
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
 * Gets the active area map at the given index id.
 * @param id Id of the map to get.
 * @return The map with the given id.
 */
Map *get_map(uint8_t id) {
  return active_area->maps + id;
}

/**
 * Loads a map in the current area with the given map_id.
 * @param map_id Id of the map to load.
 */
void load_map_old(Map *map) {
  active_map = map;

  uint8_t *vram = VRAM_BACKGROUND;
  MapTileAttribute *map_attr = map_tile_attributes;
  uint8_t *map_data = map->data;

  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(map->bank);

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

  // Set chest graphics based on their open flag state
  Chest *chest = active_area->chests;
  for (uint8_t k = 0; k < active_area->num_chests; k++, chest++) {
    if (chest->map_id != active_map->id)
      continue;
    if (check_flags(chest->flag_page, chest->open_flag)) {
      set_chest_open_graphics(chest);
    }
  }
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

/**
 * Checks to see if the player is attempting to, and can move in the desired
 * direction. If so it initiates move by updating map state.
 * @see can_move, start_map_move, stop_map_move
 */
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

/**
 * Stops map movement, turns off sprites, and begins fading the screen out.
 */
void map_fade_out(void) {
  stop_map_move();
  toggle_sprites();
  fade_out();
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
  MapTileAttribute a = get_tile_attribute();
  if (a != MAP_EXIT)
    return false;

  for (uint8_t k = 0; k < active_area->num_exits; k++) {
    Exit *x = &active_area->exits[k];
    if (x->map_id != active_map->id) continue;
    if (x->col != map_col) continue;
    if (x->row != map_row) continue;

    active_exit = x;
    map_fade_out();
    map_state = MAP_STATE_FADE_OUT;

    // TODO Add player "exiting" animation based on exit type
    // TODO Handle exits in the same map
    // TODO Handle area spanning exis
    // TODO Gracefully handle map sprites

    return true;
  }

  return false;
}

/**
 * Updates map state while the character is moving.
 */
void update_map_move_old(void) {
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

  on_move();

  // Note: map state changes based on attribute must return from this switch
  //       otherwise the state will be overwritten by `check_move` below.
  switch (get_tile_attribute()) {
  case MAP_EXIT:
    if (on_exit() || handle_exit())
      return;
    break;
  case MAP_SPECIAL:
    if (on_special())
      return;
    break;
  default:
  }

  // Immediately check for d-pad input to allow continuous movement.
  check_move();
}

/**
 * Determines if a player is attempting to open a chest and handles the logic
 * if they are.
 */
void check_chests(void) {
  Chest *chest = active_area->chests;
  for (uint8_t k = 0; k < active_area->num_chests; k++, chest++) {
    if (active_map->id != chest->map_id)
      continue;
    if (check_flags(chest->flag_page, chest->open_flag))
      continue;
    if (!player_facing(chest->col, chest->row))
      continue;
    if (before_chest(chest)) {
      set_flags(chest->flag_page, chest->open_flag);
      set_chest_open_graphics(chest);
      on_chest(chest);
    }
    return;
  }
}

/**
 * Checks to see if the player is attempting to perform an action by pressing
 * the A button.
 * @return `true` if an action was attempted.
 */
bool check_action(void) {
  if (was_pressed(J_A)) {
    check_chests();
    on_action();
    return true;
  }
  return false;
}

void set_chest_open_graphics(Chest *chest) {
  // TODO Decide on a standard position for chest graphics in the general area
  //      tileset layout, 0x2C will suffice for now.
  uint8_t *vram = VRAM_BACKGROUND_XY(chest->col * 2, chest->row * 2);
  set_vram_byte(vram, 0x2C);
  set_vram_byte(vram + 1, 0x2D);
  set_vram_byte(vram + 0x20, 0x3C);
  set_vram_byte(vram + 0x20 + 1, 0x3D);
}

void start_battle(void) {
  // TODO "Cool" battle starting animation
  map_fade_out();
  map_state = MAP_STATE_START_BATTLE;
}

// TODO This is very similar "init", I think the only real difference is
//      that init loads the area and sets the map to map_id 0
void return_from_battle(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);

  text_writer.auto_page = AUTO_PAGE_OFF;

  textbox.init();
  core.load_font();
  text_writer.auto_page = AUTO_PAGE_OFF;

  init_hero();
  load_area_graphics(active_area);
  on_init();
  load_map_old(active_map);
  set_map_xy_from_col_row();

  // TODO It is weird these are decoupled. Is there a reason?
  update_map_positions();
  move_bkg(map_scroll_x, map_scroll_y);

  fade_in();
  map_state = MAP_STATE_FADE_IN;
  game_state = GAME_STATE_WORLD_MAP;

  lcd_on();
}














//------------------------------------------------------------------------------
// END NEW CODE
//------------------------------------------------------------------------------
















void init_world_map(Area *area) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  lcd_off();

  load_area(area);

  textbox.init();
  core.load_font();
  text_writer.auto_page = AUTO_PAGE_OFF;

  init_hero();
  load_area_graphics(active_area);
  on_init();
  load_map_old(get_map(0));

  map_state = MAP_STATE_WAITING;

  lcd_on();
}

// inline void load_tile(uint8_t *map_data, uint8_t *vram, uint8_t *map_attr) {
//   // TODO This is a data loader, refactor into a core data service
//   uint8_t data = *map_data;
//   uint8_t attr = *(map_data + 1);
//   uint8_t tile = map_tile_lookup[data & MAP_TILE_MASK];

//   // Set tilemap attributes
//   VBK_REG = VBK_ATTRIBUTES;
//   *vram = attr;
//   *(vram + 1) = attr;
//   *(vram + 0x20) = attr;
//   *(vram + 0x20 + 1) = attr;

//   // Set tile from data
//   VBK_REG = VBK_TILES;
//   *vram = tile;
//   *(vram + 0x20) = tile + 16;
//   *(vram + 1) = tile + 1;
//   *(vram + 0x20 + 1) = tile + 16 + 1;

//   // Load the tile type into main memory
//   *map_attr = data >> 6;
// }

typedef struct MapTile {
  bool blank;
  uint8_t tile;
  uint8_t attr;
  uint8_t map_attr;
} MapTile;

typedef struct MapData {
  uint8_t bank;
  uint8_t *data;
  int8_t width;
  int8_t height;
} MapData;

#define MAP_HORIZ_LOADS 12
#define MAP_VERT_LOADS 11

typedef struct MapSystem {
  // Global State
  MapState state;

  // Map position
  int8_t x;
  int8_t y;

  // Movement
  int8_t scroll_x;
  int8_t scroll_y;
  Direction move_direction;
  uint8_t move_step;

  // Progressive load
  int8_t vram_x;
  int8_t vram_y;
  MapTile tile_buf[2 * MAP_HORIZ_LOADS];
  uint8_t buffer_pos;
  uint8_t buffer_max;
  int8_t vram_col;
  int8_t vram_row;
  int8_t vram_d_col;
  int8_t vram_d_row;

  // Map data
  uint8_t bank;
  uint8_t *data;
  int8_t width;
  int8_t height;
} MapSystem;

MapSystem map = { MAP_STATE_WAITING };


void get_map_tile(MapTile *tile, int8_t x, int8_t y) NONBANKED {
  if (x < 0 || y < 0 || x >= map.width || y >= map.height) {
    tile->blank = true;
    return;
  }

  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(map.bank);
  const uint16_t offset = (uint16_t)(2 * (x + y * map.width));
  const uint8_t *data = map.data + offset;
  const uint8_t t = *data++;
  const uint8_t a = *data;
  SWITCH_ROM(_prev_bank);

  tile->blank = false;
  tile->tile = map_tile_lookup[t & MAP_TILE_MASK];
  tile->attr = a;
  tile->map_attr = t >> 6;
}


void move_buffer(int8_t dx, int8_t dy) {
  // Reset the buffer position
  MapTile *tile_buf = map.tile_buf;
  map.buffer_pos = 0;

  // Update world coordinates
  map.x += dx;
  map.y += dy;

  // Update VRAM origin
  map.vram_x += 2 * dx;
  if (map.vram_x < 0)
    map.vram_x += 32;
  else if (map.vram_x >= 32)
    map.vram_x -= 32;

  map.vram_y += 2 * dy;
  if (map.vram_y < 0)
    map.vram_y += 32;
  else if (map.vram_y >= 32)
    map.vram_y -= 32;

  // Load data into buffer
  if (dy != 0) {
    map.buffer_max = MAP_HORIZ_LOADS;
    map.vram_d_col = 2;
    map.vram_d_row = 0;

    const int8_t vram_offset = dy < 0 ? 0 : 0x14;
    map.vram_col = map.vram_x;
    map.vram_row = map.vram_y + vram_offset;
    if (map.vram_row >= 0x20)
      map.vram_row -= 0x20;

    const int8_t map_offset = dy < 0 ? 0 : 10;
    for (int8_t x = map.x - 1; x < map.x + MAP_HORIZ_LOADS; x++, tile_buf++)
      get_map_tile(tile_buf, x, map.y - 1 + map_offset);
  } else if (dx != 0) {
    map.buffer_max = MAP_VERT_LOADS;
    map.vram_d_col = 0;
    map.vram_d_row = 2;

    const int8_t vram_offset = dx < 0 ? 0 : 0x16;
    map.vram_col = map.vram_x + vram_offset;
    if (map.vram_col >= 0x20)
      map.vram_col -= 0x20;
    map.vram_row = map.vram_y;

    const int8_t map_offset = dx < 0 ? -1 : 10;
    for (int8_t y = map.y - 1; y < map.y + MAP_VERT_LOADS; y++, tile_buf++)
      get_map_tile(tile_buf, map.x + map_offset, y);
  }
}

void load_tile_buffer(void) {
  MapTile *tile_buf = map.tile_buf;
  map.buffer_pos = 0;

  switch (map.move_direction) {
  case UP:
    move_buffer(0, -1);
    break;
  case DOWN:
    move_buffer(0, 1);
    break;
  case LEFT:
    move_buffer(-1, 0);
    break;
  case RIGHT:
    move_buffer(1, 0);
    break;
  }
}

void draw_map_tile(uint8_t *vram, MapTile *map_tile) {
  const uint8_t tile = map_tile->blank ? 0 : map_tile->tile;
  const uint8_t attr = map_tile->blank ? 0 :  map_tile->attr;

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
}

void draw_initial_map_screen(void) {
  uint8_t *vram = VRAM_BACKGROUND;
  MapTile tile;

  const int8_t screen_tiles_x = 10;
  const int8_t screen_tiles_y = 9;

  for (int8_t y = map.y - 1; y < map.y + screen_tiles_y + 1; y++) {
    for (int8_t x = map.x - 1; x < map.x + screen_tiles_x + 1; x++) {
      get_map_tile(&tile, x, y);
      draw_map_tile(vram, &tile);
      vram += 2;
    }
    vram += 32 + (32 - MAP_HORIZ_LOADS * 2);
  }

  map.scroll_x = 16;
  map.scroll_y = 16;
  map.vram_x = 0;
  map.vram_y = 0;
  move_bkg(map.scroll_x, map.scroll_y);
}

void start_move(Direction d) {
  map.move_direction = d;
  map.move_step = 0;
  map.state = MAP_STATE_MOVING;
  load_tile_buffer();
}

void update_map_move(void) {
  switch (map.move_direction) {
  case UP:
    map.scroll_y--;
    break;
  case DOWN:
    map.scroll_y++;
    break;
  case LEFT:
    map.scroll_x--;
    break;
  case RIGHT:
    map.scroll_x++;
    break;
  }

  if (map.buffer_pos < map.buffer_max) {
    MapTile *tile = map.tile_buf + map.buffer_pos;
    uint8_t *vram = VRAM_BACKGROUND_XY(map.vram_col, map.vram_row);
    draw_map_tile(vram, tile);

    map.buffer_pos++;

    map.vram_col += map.vram_d_col;
    if (map.vram_col >= 32)
      map.vram_col -= 32;
    map.vram_row += map.vram_d_row;
    if (map.vram_row >= 32)
      map.vram_row -= 32;
  }

  move_bkg(map.scroll_x, map.scroll_y);

  // Move complete
  if (++map.move_step == 16) {
    map.state = MAP_STATE_WAITING;
    return;
  }
}

void initialize_world_map(void) {
  lcd_off();

  // TODO: Remove at some point, just for debug
  MapTile *tile = map.tile_buf;
  for (uint8_t k = 0; k < MAP_HORIZ_LOADS; k++, tile++) {
    tile->tile = 0xCC;
    tile->attr = 0xDD;
    tile->blank = 0xEE;
    tile->map_attr = 0xFF;
  }

  map.bank = 9;
  map.data = floor_test_data;
  map.width = 32;
  map.height = 32;
  map.x = 0;
  map.y = 0;
  draw_initial_map_screen();

  textbox.init();
  core.load_font();
  text_writer.auto_page = AUTO_PAGE_OFF;

  // TODO Fix me
  VBK_REG = VBK_BANK_0;
  core.load_tileset(&dungeon_tileset, VRAM_BG_TILES);
  core.load_object_tiles();
  core.load_bg_palette(area0.palettes, 0, 4);
  toggle_sprites();
  active_area = &area0;
  active_map = area0.maps;
  init_hero();
  // load_map(get_map(0));

  // on_init();
  map_state = MAP_STATE_WAITING;

  lcd_on();
}

void init_world_map_new(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  initialize_world_map();
}


void update_world_map(void) {
  switch (map.state) {
  case MAP_STATE_WAITING:
    if (was_pressed(J_UP))
      start_move(UP);
    else if (was_pressed(J_DOWN))
      start_move(DOWN);
    else if (was_pressed(J_LEFT))
      start_move(LEFT);
    else if (was_pressed(J_RIGHT))
      start_move(RIGHT);
    break;
  case MAP_STATE_MOVING:
    update_map_move();
    break;
  }
}

void draw_world_map(void) {
}














//------------------------------------------------------------------------------
// END NEW CODE
//------------------------------------------------------------------------------















void update_world_map_old(void) {
  if (
    map_state == MAP_STATE_START_BATTLE ||
    map_state == MAP_STATE_INACTIVE
  ) {
    // Important, don't remove.
    return;
  }

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
  on_update();
}

void draw_world_map_old(void) {
  if (map_state == MAP_STATE_INACTIVE)
    return;

  switch (map_state) {
  case MAP_STATE_TEXTBOX:
    textbox.update();
    if (textbox.state == TEXT_BOX_CLOSED) {
      map_state = MAP_STATE_WAITING;
    }
    break;
  case MAP_STATE_START_BATTLE:
    if (fade_update()) {
      lcd_off();
      map_state = MAP_STATE_INACTIVE;
      init_battle();
      return;
    }
    break;
  case MAP_STATE_FADE_OUT:
    if (fade_update()) {
      map_state = MAP_STATE_LOAD;
    }
    break;
  case MAP_STATE_LOAD:
    lcd_off();
    load_map_old(get_map(active_exit->to_map));
    map_col = active_exit->to_col;
    map_row = active_exit->to_row;
    hero_direction = active_exit->heading;
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
      // TODO Animate the character based on exit type and heading
      map_state = MAP_STATE_WAITING;
      LCDC_REG ^= LCDCF_OBJON;
    }
    break;
  }
  on_draw();
}

