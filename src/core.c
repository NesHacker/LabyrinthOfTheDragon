#include <stdio.h>
#include "core.h"

uint8_t *debug = (void *)0xB000;
uint16_t *debug16 = (void *)0xB800;

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

uint8_t flags[32] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const palette_color_t blank_palette[4] = {
  RGB_WHITE,
  RGB_WHITE,
  RGB_WHITE,
  RGB_WHITE,
};

/**
 * Font tileset.
 */
static const Tileset tileset_font = { 128, 10, tile_data_font };

/**
 * Battle tileset.
 */
static const Tileset tileset_battle = { 5 * 16, 10, tile_battle };

/**
 * Common objects tileset.
 */
static const Tileset objects_tileset = { 128, 10, tile_data_objects };

/**
 * Dungeon level tileset (page 1).
 */
static const Tileset dungeon_tileset_page1 = {
  128,
  12,
  tile_data_dungeon
};

/**
 * Dungeon level tileset (page 2).
 */
static const Tileset dungeon_tileset_page2 = {
  128,
  12,
  tile_data_dungeon + BYTES_PER_TILE * 128
};

/**
 * Dungeon level tileset (page 2).
 */
static const Tileset dungeon_tileset_page3 = {
  128,
  12,
  tile_data_dungeon + BYTES_PER_TILE * 128 * 2
};

/**
 * Hero sprites tileset.
 */
static const Tileset hero_tileset = { 12 * 8, 10, tile_data_hero };

/**
 * Monster map character tiles, page1.
 */
static const Tileset monsters_tileset_page1 = {
  96, 15, tile_monsters
};

/**
 * Monster map character tiles, page2.
 */
static const Tileset monsters_tileset_page2 = {
  96, 15, tile_monsters + BYTES_PER_TILE * 96
};

/**
 * Monster map character tiles, page2.
 */
static const Tileset monsters_tileset_page3 = {
  96, 15, tile_monsters + BYTES_PER_TILE * 96 * 2
};

// 224

/**
 * ...
 */
static const Tileset title_tileset_page1 = {
  128, 1, tile_title
};

/**
 * ...
 */
static const Tileset title_tileset_page2 = {
  96, 1, tile_title + BYTES_PER_TILE * 128
};

/**
 * ...
 */
static const Tileset title_tileset_fire = {
  80, 1, tile_title_fire
};

/**
 * ...
 */
static const Tileset title_tileset_smoke = {
  48, 1, tile_title_smoke
};

/**
 * ---
 */
static const Tileset neshacker_presents = {
  32, 1, tile_neshacker_presents
};

static void load_tileset(const Tileset *s, uint8_t *dst) NONBANKED {
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

static void core_load_tiles(
  const Tileset *s,
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

static void load_battle_tiles(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&tileset_battle, VRAM_BATTLE_TILES);
}

static void load_font(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&tileset_font, VRAM_FONT_TILES);
}

static void load_dungeon_tiles(void) NONBANKED {
  VBK_REG = VBK_BANK_0;
  core.load_tileset(&dungeon_tileset_page1, VRAM_BG_TILES);
  VBK_REG = VBK_BANK_0;
  core.load_tileset(&dungeon_tileset_page2, VRAM_SHARED_TILES);
  VBK_REG = VBK_BANK_1;
  core.load_tileset(&dungeon_tileset_page3, VRAM_BG_TILES);
}

static void load_hero_tiles(uint8_t player_class) NONBANKED {
  const uint8_t row_width = 12;
  const uint8_t offset = player_class * row_width * 2;
  const uint8_t offset2 = offset + row_width;
  uint8_t *const vram = VRAM_SPRITE_TILES;
  uint8_t *const vram2 = vram + 16 * BYTES_PER_TILE;

  VBK_REG = VBK_BANK_0;
  core_load_tiles(&hero_tileset, vram, offset, row_width);
  core_load_tiles(&hero_tileset, vram2, offset2, row_width);
}

static void load_all_heros(void) NONBANKED {
  VBK_REG = VBK_BANK_0;

  uint8_t *vram = VRAM_SPRITE_TILES;
  for (uint8_t player_class = 0; player_class < 4; player_class++) {
    const uint8_t row_width = 12;
    const uint8_t offset = player_class * row_width * 2;
    const uint8_t offset2 = offset + row_width;

    uint8_t *const vram2 = vram + 16 * BYTES_PER_TILE;
    core_load_tiles(&hero_tileset, vram, offset, row_width);
    core_load_tiles(&hero_tileset, vram2, offset2, row_width);

    vram += 0x20 * BYTES_PER_TILE;
  }
}

static void load_object_tiles(void) NONBANKED {
  VBK_REG = VBK_BANK_1;
  load_tileset(&objects_tileset, VRAM_SPRITE_TILES);
}

static void draw_tilemap(Tilemap m, uint8_t *dst) NONBANKED {
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

static void load_bg_palette(const palette_color_t *data, uint8_t index, uint8_t n) {
  update_bg_palettes(index, n, data);
}

static void load_sprite_palette(const palette_color_t *data, uint8_t index, uint8_t n) {
  update_sprite_palettes(index, n, data);
}

static void fill_bg(uint8_t tile_id, uint8_t attr) NONBANKED {
  uint8_t *vram = VRAM_BACKGROUND;
  for (uint16_t k = 0; k < 32 * 32; k++) {
    VBK_REG = VBK_TILES;
    *vram = tile_id;
    VBK_REG = VBK_ATTRIBUTES;
    *vram++ = attr;
  }
}

static void draw_text(uint8_t *vram, const char *text, uint8_t max) NONBANKED {
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

static void print_fraction(uint8_t *vram, uint16_t n, uint16_t d) {
  char buf[16];
  sprintf(buf, "%u/%u", n, d);
  draw_text(vram, buf, 7);
}

static void core_fill(
  uint8_t *vram,
  uint8_t w,
  uint8_t h,
  uint8_t tile,
  uint8_t attr
) {
  for (uint8_t y = 0; y < h; y++, vram += 32 - w) {
    for (uint8_t x = 0; x < w; x++, vram++) {
      VBK_REG = VBK_TILES;
      set_vram_byte(vram, tile);
      VBK_REG = VBK_ATTRIBUTES;
      set_vram_byte(vram, attr);
    }
  }
}

static void load_monster_tiles(MonsterTiles tiles, MonsterTilePosition pos) {
  const Tileset *tileset;

  switch (tiles) {
  case MONSTER_TILES_KOBOLD:
  case MONSTER_TILES_GOBLIN:
  case MONSTER_TILES_ZOMBIE:
  case MONSTER_TILES_BUGBEAR:
    tileset = &monsters_tileset_page1;
    break;
  case MONSTER_TILES_OWLBEAR:
  case MONSTER_TILES_GELATINOUS_CUBE:
  case MONSTER_TILES_WILL_O_WISP:
  case MONSTER_TILES_DISPLACER_BEAST:
    tileset = &monsters_tileset_page2;
    tiles -= 4;
    break;
  default:
    tiles -= 8;
    tileset = &monsters_tileset_page3;
  }

  const uint8_t row_width = 12;
  const uint8_t offset = tiles * row_width * 2;
  const uint8_t offset2 = offset + row_width;

  uint8_t *vram = (void *)(0x8000 + BYTES_PER_TILE * (0x20 + 0x20 * pos));
  uint8_t *vram2 = vram + 16 * BYTES_PER_TILE;

  VBK_REG = VBK_BANK_0;
  core_load_tiles(tileset, vram, offset, row_width);
  core_load_tiles(tileset, vram2, offset2, row_width);
}

void load_title_tiles(void) {
  VBK_REG = VBK_BANK_0;

  uint8_t *vram1 = (void *)(0x9000);
  core_load_tiles(&title_tileset_page1, vram1, 0, 128);
  uint8_t *vram2 = (void *)(0x8800);
  core_load_tiles(&title_tileset_page2, vram2, 0, 128);

  VBK_REG = VBK_BANK_1;
  uint8_t *vram_fire = (void *)(0x8000);
  core_load_tiles(&title_tileset_fire, vram_fire, 0, 80);

  uint8_t *vram_smoke = (void *)(0x8000 + BYTES_PER_TILE * 80);
  core_load_tiles(&title_tileset_smoke, vram_smoke, 0, 48);


  core_load_tiles(&neshacker_presents, vram1, 0, 32);
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
  fill_bg,
  draw_text,
  print_fraction,
  core_fill,
  load_dungeon_tiles,
  load_monster_tiles,
  load_title_tiles,
};
