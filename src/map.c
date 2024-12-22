#pragma bank 2

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "battle.h"
#include "core.h"
#include "floor.h"
#include "map.h"

uint8_t *debug = (void *)0xA000;

MapSystem map = { MAP_STATE_WAITING };

/**
 * Initializes the hero character sprites.
 */
void init_hero(void) {
  const uint8_t hero_x = 16 * HERO_X_OFFSET;
  const uint8_t hero_y = 16 * HERO_Y_OFFSET;
  const uint8_t offset = 0x00;
  const uint8_t frame = (map.walk_frame << 1) + offset;

  init_timer(map.walk_timer, 8);

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

  // TODO Load palettes/hero sprites based on class
  core.load_hero_tiles(1);
  const uint16_t hpal[4] = {
    RGB(0, 0, 0),
    RGB8(245, 213, 135),
    RGB8(167, 75, 0),
    RGB8(8, 46, 54),
  };
  core.load_sprite_palette(hpal, 0, 1);
}

/**
 * Updates the hero sprites each frame.
 */
void update_hero(void) {
  const uint8_t hero_x = 16 * HERO_X_OFFSET;
  const uint8_t hero_y = 16 * HERO_Y_OFFSET;
  if (map.hero_state == HERO_WALKING) {
    if (update_timer(map.walk_timer)) {
      reset_timer(map.walk_timer);
      map.walk_frame ^= 1;
    }
  } else {
    map.walk_frame = 0;
  }

  uint8_t offset = 0x00;
  switch (map.hero_direction) {
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

  const uint8_t frame = (map.walk_frame << 1) + offset;

  if (map.hero_direction != LEFT) {
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

/**
 * Clears hero sprites.
 */
void clear_hero(void) {
  move_sprite(0, 0, 0);
  move_sprite(1, 0, 0);
  move_sprite(2, 0, 0);
  move_sprite(3, 0, 0);
}

/**
 * Initiates a fade out to state transition.
 * @param to_state Map state to enter once the fade out completes.
 */
void map_fade_out(MapState to_state) {
  toggle_sprites();
  map.fade_to_state = to_state;
  fade_out();
  map.state = MAP_STATE_FADE_OUT;
}

/**
 * Initiates a fade in to state transition.
 * @param to_state Map state to enter once the fade in completes.
 */
void map_fade_in(MapState to_state) {
  map.fade_to_state = to_state;
  fade_in();
  map.state = MAP_STATE_FADE_IN;
}

/**
 * Loads a map data tile at the given position in the current map.
 *
 * Note: this routine can safely be used with coordinates outside the current
 * map's dimensions. A blank tile will be returned in this case.
 *
 * @param tile Pointer to a map tile where the data should be loaded.
 * @param x X-position in the map.
 * @param y Y-position in the map.
 */
void get_map_tile(MapTile *tile, int8_t x, int8_t y) NONBANKED {
  const int8_t w = (int8_t)map.active_map->width;
  const int8_t h = (int8_t)map.active_map->height;

  if (x < 0 || y < 0 || x >= w || y >= h) {
    tile->blank = true;
    return;
  }

  const uint16_t offset = (uint16_t)(2 * (x + y * w));
  const uint8_t *data = map.active_map->data + offset;

  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(map.active_map->bank);
  const uint8_t t = *data++;
  const uint8_t a = *data;
  SWITCH_ROM(_prev_bank);

  tile->blank = false;
  tile->tile = map_tile_lookup[t & MAP_TILE_MASK];
  tile->attr = a;
  tile->map_attr = t >> 6;
}

/**
 * Fills the progressive load tile buffer with data depending on the direction
 * the player is currently moving.
 */
void load_tile_buffer(void) {
  // Set horizontal and vertical deltas based on move direction
  int8_t dx, dy;
  switch (map.move_direction) {
  case UP:
    dx = 0;
    dy = -1;
    break;
  case DOWN:
    dx = 0;
    dy = 1;
    break;
  case LEFT:
    dx = -1;
    dy = 0;
    break;
  case RIGHT:
    dx = 1;
    dy = 0;
    break;
  default:
    return;
  }

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

/**
 * Draws a map tile at the given location in VRAM.
 * @param vram Position in VRAM where the tile should be drawn.
 * @param map_tile Map tile data to draw.
 */
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

/**
 * Draws a full screen with a border of prebuffered tiles for the current map
 * position. This needs to be called when loading a map, transition in and out
 * of the map system, etc. as it sets initial positions and data required for
 * progressive loading of tiles based on movement.
 */
void refresh_map_screen(void) {
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

/**
 * Initiates a progressive screen reload based on the active exit being taken
 * by the player.
 */
void load_exit(void) {
  lcd_off();
  Exit *exit = map.active_exit;
  map.active_map = map.active_floor->maps + exit->to_map;
  map.hero_direction = exit->heading;
  set_hero_position(exit->to_col, exit->to_row);
  refresh_map_screen();
  map_fade_in(MAP_STATE_EXIT_LOADED);
  lcd_on();
}

/**
 * Handles state updates when the player moves into an exit tile.
 * @return `true` if default move behavior should be prevented.
 */
bool handle_exit(void) {
  uint8_t x = map.x + HERO_X_OFFSET;
  uint8_t y = map.y + HERO_Y_OFFSET;

  Exit *exit = map.active_floor->exits;
  for (uint8_t k = 0; k < map.active_floor->num_exits; k++, exit++) {
    if (exit->map_id != map.active_map->id)
      continue;
    if (exit->col != x || exit->row != y)
      continue;

    *debug++ = 0xAC;
    *debug++ = 0xDC;

    map.active_exit = exit;
    map_fade_out(MAP_STATE_LOAD_EXIT);
    return true;
  }

  return false;
}

/**
 * Initiates player movement in the given direction if possible.
 * @param d Direction the player is to move.
 */
void start_move(Direction d) {
  MapTile *destination = map.local_tiles + d;

  map.hero_direction = d;

  if (destination->map_attr == MAP_WALL) {
    return;
  }

  map.move_direction = d;
  map.move_step = 0;
  map.state = MAP_STATE_MOVING;
  map.hero_state = HERO_WALKING;
  load_tile_buffer();
}

/**
 * Checks user input and initiates a map move if they're holding the d-pad in
 * a given direction.
 */
void check_map_move(void) {
  if (is_down(J_UP))
    start_move(UP);
  else if (is_down(J_DOWN))
    start_move(DOWN);
  else if (is_down(J_LEFT))
    start_move(LEFT);
  else if (is_down(J_RIGHT))
    start_move(RIGHT);
}

/**
 * Updates the local tiles state based on the current map position.
 */
void update_local_tiles(void) {
  uint8_t x = map.x + HERO_X_OFFSET;
  uint8_t y = map.y + HERO_Y_OFFSET;
  get_map_tile(map.local_tiles + HERE, x, y);
  get_map_tile(map.local_tiles + UP, x, y - 1);
  get_map_tile(map.local_tiles + DOWN, x, y + 1);
  get_map_tile(map.local_tiles + LEFT, x - 1, y);
  get_map_tile(map.local_tiles + RIGHT, x + 1, y);
}

/**
 * Update handler for when the player is moving.
 */
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

  if (++map.move_step < 16)
    return;

  update_local_tiles();

  map.state = MAP_STATE_WAITING;
  map.hero_state = HERO_STILL;

  if (on_move())
    return;

  MapTile *here = map.local_tiles + HERE;

  switch (here->map_attr) {
  case MAP_EXIT:
    if (on_exit())
      return;
    if (handle_exit())
      return;
    break;
  case MAP_SPECIAL:
    if (on_special())
      return;
    break;
  default:
  }

  check_map_move();
}

void set_active_floor(Floor *floor) BANKED {
  map.active_floor = floor;
  map.active_map = &floor->maps[floor->default_map];
  set_hero_position(floor->default_x, floor->default_y);
}

/**
 * Initializes the world map system.
 */
void initialize_world_map(void) {
  lcd_off();

  core.load_font();
  core.load_object_tiles();
  core.load_dungeon_tiles();
  core.load_bg_palette(map.active_floor->palettes, 0, 7);

  text_writer.auto_page = AUTO_PAGE_OFF;
  textbox.init();

  init_hero();
  update_local_tiles();
  refresh_map_screen();

  lcd_on();
}

void init_world_map(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  initialize_world_map();
  map.state = MAP_STATE_WAITING;
}

void start_battle(void) {
  map_fade_out(MAP_STATE_START_BATTLE);
}

void return_from_battle(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  game_state = GAME_STATE_WORLD_MAP;
  initialize_world_map();
  map_fade_in(MAP_STATE_WAITING);
}

void update_world_map(void) NONBANKED {
  switch (map.state) {
  case MAP_STATE_INACTIVE:
    return;
  case MAP_STATE_WAITING:
    check_map_move();
    break;
  case MAP_STATE_MOVING:
    update_map_move();
    break;
  case MAP_STATE_FADE_OUT:
    if (fade_update()) {
      map.state = map.fade_to_state;
    }
    break;
  case MAP_STATE_FADE_IN:
    if (fade_update()) {
      toggle_sprites();
      map.state = map.fade_to_state;
    }
    break;
  case MAP_STATE_LOAD_EXIT:
    load_exit();
    break;
  case MAP_STATE_EXIT_LOADED:
    start_move(map.active_exit->heading);
    break;
  case MAP_STATE_START_BATTLE:
    map.state = MAP_STATE_INACTIVE;
    lcd_off();
    init_battle();
    return;
  }

  update_hero();
}

void draw_world_map(void) {
}

//------------------------------------------------------------------------------
// BEGIN OLD CODE
//------------------------------------------------------------------------------




// /**
//  * Determines if a player is attempting to open a chest and handles the logic
//  * if they are.
//  */
// void check_chests(void) {
//   Chest *chest = active_area->chests;
//   for (uint8_t k = 0; k < active_area->num_chests; k++, chest++) {
//     if (active_map->id != chest->map_id)
//       continue;
//     if (check_flags(chest->flag_page, chest->open_flag))
//       continue;
//     if (!player_facing(chest->col, chest->row))
//       continue;
//     if (before_chest(chest)) {
//       set_flags(chest->flag_page, chest->open_flag);
//       set_chest_open_graphics(chest);
//       on_chest(chest);
//     }
//     return;
//   }
// }

// /**
//  * Checks to see if the player is attempting to perform an action by pressing
//  * the A button.
//  * @return `true` if an action was attempted.
//  */
// bool check_action(void) {
//   if (was_pressed(J_A)) {
//     check_chests();
//     on_action();
//     return true;
//   }
//   return false;
// }

// void start_battle(void) {
//   // TODO "Cool" battle starting animation
//   map_fade_out();
//   map_state = MAP_STATE_START_BATTLE;
// }

// void update_world_map_old(void) {
//   if (
//     map_state == MAP_STATE_START_BATTLE ||
//     map_state == MAP_STATE_INACTIVE
//   ) {
//     // Important, don't remove.
//     return;
//   }

//   switch (map_state) {
//   case MAP_STATE_WAITING:
//     if (!check_action()) {
//       check_move();
//     }
//     update_hero();
//     move_bkg(map_scroll_x, map_scroll_y);
//     break;
//   case MAP_STATE_MOVING:
//     update_map_move();
//     break;
//   }
//   on_update();
// }

// void draw_world_map_old(void) {
//   if (map_state == MAP_STATE_INACTIVE)
//     return;

//   switch (map_state) {
//   case MAP_STATE_TEXTBOX:
//     textbox.update();
//     if (textbox.state == TEXT_BOX_CLOSED) {
//       map_state = MAP_STATE_WAITING;
//     }
//     break;
//   case MAP_STATE_START_BATTLE:
//     if (fade_update()) {
//       lcd_off();
//       map_state = MAP_STATE_INACTIVE;
//       init_battle();
//       return;
//     }
//     break;
//   case MAP_STATE_FADE_OUT:
//     if (fade_update()) {
//       map_state = MAP_STATE_LOAD;
//     }
//     break;
//   case MAP_STATE_LOAD:
//     lcd_off();
//     load_map_old(get_map(active_exit->to_map));
//     map_col = active_exit->to_col;
//     map_row = active_exit->to_row;
//     hero_direction = active_exit->heading;
//     set_map_xy_from_col_row();
//     update_map_positions();
//     update_hero();
//     move_bkg(map_scroll_x, map_scroll_y);
//     map_state = MAP_STATE_FADE_IN;
//     fade_in();
//     lcd_on();
//     break;
//   case MAP_STATE_FADE_IN:
//     if (fade_update()) {
//       // TODO Animate the character based on exit type and heading
//       map_state = MAP_STATE_WAITING;
//       LCDC_REG ^= LCDCF_OBJON;
//     }
//     break;
//   }
//   on_draw();
// }

