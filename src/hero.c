#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "global.h"
#include "hero.h"
#include "util.h"

void init_hero(void) {
  const uint8_t x = 0x80;
  const uint8_t y = 0x20;
  const uint8_t offset = 0x00;
  const uint8_t frame = (walk_frame << 1) + offset;

  init_timer(walk_timer, 18);

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
  if (update_timer(walk_timer)) {
    reset_timer(walk_timer);
    walk_frame ^= 1;
  }
  const uint8_t offset = 0x00;
  const uint8_t frame = (walk_frame << 1) + offset;
  set_sprite_tile(0, frame);
  set_sprite_tile(1, frame + 0x01);
  set_sprite_tile(2, frame + 0x10);
  set_sprite_tile(3, frame + 0x11);
}

