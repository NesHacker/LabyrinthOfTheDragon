#pragma bank 3

#include "battle.h"
#include "data.h"
#include "monster.h"
#include "stats.h"

void reset_monster_instance(MonsterInstance *m) {
  m->monster = NULL;
  m->palette = NULL;
  m->id = '\0';
  m->exp_tier = C_TIER;
  m->hp = 0;
  m->max_hp = 0;
  m->atk = 0;
  m->def = 0;
  m->matk = 0;
  m->mdef = 0;
  m->agl = 0;
  m->aspect_immune = 0;
  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;
}

// -----------------------------------------------------------------------------
// Monster 1 - Kobold
// -----------------------------------------------------------------------------
const palette_color_t MONSTER_KOBOLD_PALETTES[16] = {
  // C-Tier
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // B-Tier
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // A-Tier
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // S-Tier
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
};

void kobold_new_instance(uint8_t pos, uint8_t level, PowerTier tier) {
  MonsterInstance *m = get_monster_at(pos);
  reset_monster_instance(m);
  m->monster = &MONSTER_KOBOLD;
  m->palette = MONSTER_KOBOLD_PALETTES;
  m->level = level;
  m->exp_tier = tier;

  // TODO Implement me
  // switch (tier) {
  // case C_TIER:
  //   break;
  // case B_TIER:
  //   break;
  // case A_TIER:
  //   break;
  // case S_TIER:
  //   break;
  // }
}

void kobold_take_turn(uint8_t pos) {
  // TODO Implement me
}

const Monster MONSTER_KOBOLD = {
  // id, name
  1, "KOBOLD",
  // tile_bank, tile_data, palette
  4, tile_kobold,
  // Callbacks
  kobold_new_instance,
  kobold_take_turn,
};

// -----------------------------------------------------------------------------
// Monster 2 - Beholder
// -----------------------------------------------------------------------------
const palette_color_t MONSTER_BEHOLDER_PALETTES[4] = {
  // C-Tier
  RGB_WHITE,
  RGB8(209, 206, 107),
  RGB8(126, 73, 73),
  RGB8(0, 40, 51),
};

void beholder_new_instance(uint8_t pos, uint8_t level, PowerTier tier) {
  MonsterInstance *m = get_monster_at(pos);
  reset_monster_instance(m);
  m->monster = &MONSTER_BEHOLDER;
  m->palette = MONSTER_BEHOLDER_PALETTES;
  m->level = level;
  m->exp_tier = tier;

  // TODO Implement me
  // switch (tier) {
  // case C_TIER:
  //   break;
  // case B_TIER:
  //   break;
  // case A_TIER:
  //   break;
  // case S_TIER:
  //   break;
  // }
}

void beholder_take_turn(uint8_t pos) {
  // TODO Implement me
}

const Monster MONSTER_BEHOLDER = {
  // id, name
  2, "BEHOLDER",
  // tile_bank, tile_data, palette
  4, tile_beholder,
  // Callbacks
  beholder_new_instance,
  beholder_take_turn,
};
