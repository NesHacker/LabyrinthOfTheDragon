#pragma bank 0

#include "core.h"
#include "data.h"
#include "monster.h"

//------------------------------------------------------------------------------
// Tilesets
//------------------------------------------------------------------------------

const Tileset test_dummy_tileset = { MONSTER_TILES, 14, tile_dummy };
const Tileset kobold_tileset = { MONSTER_TILES, 14, tile_kobold };
const Tileset goblin_tileset = { MONSTER_TILES, 14, tile_goblin };
const Tileset zombie_tileset = { MONSTER_TILES, 14, tile_zombie };
const Tileset bugbear_tileset = { MONSTER_TILES, 14, tile_bugbear };
const Tileset owlbear_tileset = { MONSTER_TILES, 14, tile_owlbrear };
const Tileset gelatinous_cube_tileset = { MONSTER_TILES, 14, tile_gelatinous_cube };
const Tileset displacer_beast_tileset = { MONSTER_TILES, 14, tile_displacer_beast };
const Tileset will_o_wisp_tileset = { MONSTER_TILES, 14, tile_will_o_wisp };
const Tileset deathknight_tileset = { MONSTER_TILES, 14, tile_deathknight };
const Tileset mindflayer_tileset = { MONSTER_TILES, 15, tile_mindflayer };
const Tileset beholder_tileset = { MONSTER_TILES, 15, tile_beholder };
const Tileset dragon_tileset = { MONSTER_TILES, 15, tile_dragon };

//------------------------------------------------------------------------------
// Palettes
//------------------------------------------------------------------------------

const palette_color_t dummy_palette[4] = {
  RGB_WHITE,
  RGB8(210, 0, 150),
  RGB_DARKBLUE,
  RGB8(24, 0, 24),
};

const palette_color_t kobold_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // B-Tier
  RGB_WHITE,
  RGB8(113, 51, 177),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t goblin_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t zombie_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t bugbear_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t owlbear_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t gelatinous_cube_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t displacer_beast_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t will_o_wisp_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t deathknight_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t mindflayer_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t beholder_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};

const palette_color_t dragon_palettes[16] = {
  // C-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // B-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};
