#pragma bank 2

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "core.h"
#include "data.h"
#include "flags.h"
#include "joypad.h"
#include "map.h"
#include "util.h"

// TODO Put me somewhere ---V



// TODO ^--- Put me somewhere!



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

// External area data references
// Might want to just define the area list in this file, since I think they will
// only be referenced here.
extern Area area0;

// TODO This is in core right now, perhaps pull data loading out into a factory?
extern const uint8_t map_tile_lookup[];

void init_hero(void) {
  // const uint8_t x = 8 + 16 * 4;
  // const uint8_t y = 16 + 16 * 4;

  const uint8_t offset = 0x00;
  const uint8_t frame = (walk_frame << 1) + offset;

  // TODO Load the sprites for the player's class instead of just the fighter
  core.load_hero_tiles(1);

  // TODO Setup hero palettes
  const uint16_t hpal[4] = {
    RGB(0, 0, 0),
    RGB8(245, 213, 135),
    RGB8(167, 75, 0),
    RGB8(8, 46, 54),
  };
  set_sprite_palette(0, 1, hpal);

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

/**
 * Loads an area state and assets and initializes the world map controller.
 * @param a Area to load.
 */
void load_area(Area *a) {
  // TODO This is a data loader, refactor into a core data service

  // Initialize area state
  active_area = a;
  map_col = a->default_start_column;
  map_row = a->default_start_row;
  map_state = MAP_STATE_WAITING;
  set_map_xy_from_col_row();
  update_map_positions();

  // Load tilesets and palettes
  VBK_REG = VBK_BANK_0;

  // TODO Area tilesets need a revamp now called "Environments" and you can
  //      load up to 3 at a time.
  core.load_tileset(a->tileset, VRAM_BG_TILES);
  core.load_object_tiles();
  core.load_bg_palette(a->palettes, 0, 4);

  on_init();
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
 * Loads a map in the current area with the given map_id.
 * @param map_id Id of the map to load.
 */
void load_map(uint8_t map_id) NONBANKED {
  // TODO This is a data loader, refactor into a core data service
  Map *map = &active_area->maps[map_id];
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
    stop_map_move();
    LCDC_REG ^= LCDCF_OBJON;
    map_state = MAP_STATE_FADE_OUT;
    fade_out();

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

  switch (get_tile_attribute()) {
  case MAP_EXIT:
    handle_exit();
    break;
  default:
    // Immediately check for d-pad input to allow continuous movement.
    check_move();
  }
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

void init_world_map(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  lcd_off();

  textbox.init();
  core.load_font();

  init_hero();
  load_area(&area0);
  load_map(0);

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
  on_update();
}

void draw_world_map(void) {
  switch (map_state) {
  case MAP_STATE_TEXTBOX:
    textbox.update();
    if (textbox.state == TEXT_BOX_CLOSED) {
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
    load_map(active_exit->to_map);
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
      // TODO Animate the character based on heading type
      map_state = MAP_STATE_WAITING;
      LCDC_REG ^= LCDCF_OBJON;
    }
    break;
  }
  on_draw();
}
