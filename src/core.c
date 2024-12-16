#include <stdio.h>
#include "core.h"

// TODO NONE OF THESE BELONG HERE

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


// --- These, however, do...

uint8_t flags[32] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


/**
 * Font tileset.
 */
const Tileset tileset_font = { 128, 10, tile_data_font };

/**
 * Battle tileset.
 */
const Tileset tileset_battle = { 5 * 16, 10, tile_battle };

/**
 * Common objects tileset.
 */
const Tileset objects_tileset = { 128, 10, tile_data_objects };

/**
 * Hero sprites tileset.
 */
Tileset hero_tileset = { 12 * 8, 10, tile_data_hero };

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

void load_battle_tiles(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&tileset_battle, VRAM_BATTLE_TILES);
}

void load_font(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&tileset_font, VRAM_FONT_TILES);
}

void load_hero_tiles(uint8_t player_class) NONBANKED {
  const uint8_t row_width = 12;
  const uint8_t offset = player_class * row_width * 2;
  const uint8_t offset2 = offset + row_width;
  uint8_t *const vram = VRAM_SPRITE_TILES;
  uint8_t *const vram2 = vram + 16 * BYTES_PER_TILE;

  VBK_REG = VBK_BANK_0;
  core_load_tiles(&hero_tileset, vram, offset, row_width);
  core_load_tiles(&hero_tileset, vram2, offset2, row_width);
}

void load_all_heros(void) NONBANKED {
  const uint8_t row = 4 * 3;
  VBK_REG = VBK_BANK_0;
  for (uint8_t k = 0; k < 4; k++) {
    const uint8_t offset = k * row * 2;
    uint8_t *vram = VRAM_SPRITE_TILES + 0x20 * k;
    core_load_tiles(&hero_tileset, vram, offset, row);
    core_load_tiles(&hero_tileset, vram, offset + row, row);
  }
}

void load_object_tiles(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&objects_tileset, VRAM_SPRITE_TILES);
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

void load_bg_palette(palette_color_t *data, uint8_t index, uint8_t n) {
  update_bg_palettes(index, n, data);
}

void load_sprite_palette(palette_color_t *data, uint8_t index, uint8_t n) {
  update_sprite_palettes(index, n, data);
}

void clear_bg(void) NONBANKED {
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *vram = 0;
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = 0;
  }
}

void fill_bg(uint8_t tile_id, uint8_t attr) NONBANKED {
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *vram = tile_id;
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = attr;
  }
}

void draw_text(uint8_t *vram, const char *text, uint8_t max) NONBANKED {
  VBK_REG = VBK_TILES;
  while (*text != 0 && max) {
    set_vram_byte(vram++, (*text++) + FONT_OFFSET);
    max--;
  }
  while (max) {
    set_vram_byte(vram++, FONT_SPACE);
    max--;
  }
}

void print_fraction(uint8_t *vram, uint16_t n, uint16_t d) {
  char buf[10];
  sprintf(buf, " %3u/%u", n, d);
  draw_text(vram, buf, 7);
}

const Core core = {
  load_tileset,
  core_load_tiles,
  load_battle_tiles,
  load_font,
  load_hero_tiles,
  load_all_heros,
  load_object_tiles,
  draw_tilemap,
  load_bg_palette,
  load_sprite_palette,
  clear_bg,
  fill_bg,
  draw_text,
  print_fraction,
};
