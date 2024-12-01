#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "global.h"
#include "hero.h"
#include "util.h"

HeroState hero_state = HERO_STILL;
Direction hero_direction = DOWN;

void init_hero(void) {
  const uint8_t x = 8 + 16 * 4;
  const uint8_t y = 16 + 16 * 4;
  const uint8_t offset = 0x00;
  const uint8_t frame = (walk_frame << 1) + offset;

  VBK_REG = VBK_BANK_0;
  load_tile_page(1, tile_data_hero, VRAM_SPRITE_TILES);  

  const uint16_t hpal[4] = {
    RGB(0, 0, 0),
    RGB8(245, 213, 135),
    RGB8(167, 75, 0),
    RGB8(8, 46, 54),
  };
  set_sprite_palette(0, 1, hpal);

  init_timer(walk_timer, 8);

  set_sprite_tile(0, frame);
  set_sprite_prop(0, 0);
  move_sprite(0, x, y);

  set_sprite_tile(1, frame + 0x01);
  set_sprite_prop(1, 0);
  move_sprite(1, x + 8, y);

  set_sprite_tile(2, frame + 0x10);
  set_sprite_prop(2, 0);
  move_sprite(2, x, y + 8);

  set_sprite_tile(3, frame + 0x11);
  set_sprite_prop(3, 0);
  move_sprite(3, x + 8, y + 8);
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
}

void clear_hero(void) {
  move_sprite(0, 0, 0);
  move_sprite(1, 0, 0);
  move_sprite(2, 0, 0);
  move_sprite(3, 0, 0);
}
