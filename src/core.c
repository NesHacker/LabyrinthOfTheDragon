#include <gb/gb.h>
#include <gb/cgb.h>

#include "core.h"
#include "data.h"
#include "palette.h"

/**
 * Font tileset.
 */
const Tileset tileset_font = { 128, 10, tile_data_font };

/**
 * Battle tileset.
 */
const Tileset tileset_battle = { 5 * 16, 10, tile_battle };

void load_tileset(const Tileset *s, uint8_t *dst) NONBANKED {
  const uint8_t *src = s->data;
  uint8_t size = s->size;
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(s->bank);

  do {
    for (uint8_t k = 0; k < BYTES_PER_TILE; k++)
      *dst++ = *src++;
  } while (--size);

  SWITCH_ROM(_prev_bank);
}

void core_load_tiles(
  Tileset *s,
  uint8_t *dst,
  uint8_t o,
  uint8_t n
) NONBANKED {
  const uint8_t *src = s->data + o * BYTES_PER_TILE;
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(s->bank);

  do {
    for (uint8_t k = 0; k < BYTES_PER_TILE; k++)
      *dst++ = *src++;
  } while (--n);

  SWITCH_ROM(_prev_bank);
}

void load_font(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&tileset_font, VRAM_FONT_TILES);
}

void load_battle_tiles(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&tileset_battle, VRAM_BATTLE_TILES);
}

void draw_tilemap(Tilemap m, uint8_t *dst) NONBANKED {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(m.bank);

  const uint8_t *src = m.data;
  for (uint8_t y = 0; y < m.height; y++, dst += 32 - m.width) {
    for (uint8_t x = 0; x < m.width; x++, dst++) {
      VBK_REG = VBK_TILES;
      *dst = *src++;
      VBK_REG = VBK_ATTRIBUTES;
      *dst = *src++;
    }
  }

  SWITCH_ROM(_prev_bank);
}

void load_bg_palette(Palette p, uint8_t index, uint8_t n) {
  uint8_t _prev_bank = _current_bank;
  const uint8_t *data = p.data;
  SWITCH_ROM(p.bank);
  update_bg_palettes(index, n, data);
  SWITCH_ROM(_prev_bank);
}

void load_sprite_palette(Palette p, uint8_t index, uint8_t n) {
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(p.bank);
  update_sprite_palettes(index, n, p.data);
  SWITCH_ROM(_prev_bank);
}

const Core core = {
  load_tileset,
  core_load_tiles,
  load_font,
  load_battle_tiles,
  draw_tilemap,
  load_bg_palette,
  load_sprite_palette,
};
