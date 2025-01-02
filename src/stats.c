#pragma bank 5

#include "stats.h"
#include "player.h"

uint16_t get_exp(uint8_t level) BANKED {
  return exp_by_level[level - 1];
}

uint16_t get_monster_exp(uint8_t level, PowerTier tier) BANKED {
  return monster_exp[tier][level - 1];
}

uint8_t get_agl(uint8_t level, PowerTier tier) BANKED {
  return agl[tier][level - 1];
}

uint16_t get_player_hp(uint8_t level, PowerTier tier) BANKED {
  return player_hp[tier][level - 1];
}

uint8_t get_player_sp(uint8_t level, PowerTier tier) BANKED {
  return player_sp[tier][level - 1];
}

uint8_t get_player_def(uint8_t level, PowerTier tier) BANKED {
  return player_def[tier][level - 1];
}

uint8_t get_player_atk(uint8_t level, PowerTier tier) BANKED {
  return player_atk[tier][level - 1];
}

uint16_t get_player_damage(uint8_t level, PowerTier tier) BANKED {
  return player_dmg[tier][level - 1];
}

uint16_t get_monster_hp(uint8_t level, PowerTier tier) BANKED {
  return monster_hp[tier][level - 1];
}

uint8_t get_monster_def(uint8_t level, PowerTier tier) BANKED {
  return monster_def[tier][level - 1];
}

uint8_t get_monster_atk(uint8_t level, PowerTier tier) BANKED {
  return monster_atk[tier][level - 1];
}

uint16_t get_monster_dmg(uint8_t level, PowerTier tier) BANKED {
  return monster_dmg[tier][level - 1];
}

uint16_t get_player_heal(uint8_t level, PowerTier tier) BANKED {
  return player_heal[tier][level - 1];
}

bool check_attack(uint8_t d256_roll, uint8_t atk, uint8_t def) BANKED {
  int8_t delta = atk - def;
  if (delta <= -32)
    return d256_roll < attack_roll_target[0];
  if (delta >= 32)
    return d256_roll < attack_roll_target[64];
  return d256_roll < attack_roll_target[(uint8_t)(delta + 32)];
}

bool roll_attack(uint8_t atk, uint8_t def) BANKED {
  return check_attack(d256(), atk, def);
}

uint16_t calc_damage(uint8_t d16_roll, uint16_t base_dmg) BANKED {
  uint16_t d = base_dmg;
  d *= (uint16_t)damage_roll_modifier[d16_roll & 0x0F];
  d /= 16;
  return d == 0 ? 1 : d;
}

uint16_t calc_monster_exp(uint8_t mlevel, PowerTier tier) BANKED {

  int8_t diff = (int8_t)mlevel - (int8_t)player.level;

  if (diff <= -8)
    return 0;

  if (diff >= 7)
    return get_monster_exp(mlevel, tier) << 1;

  uint16_t k = (uint16_t)xp_mod[diff + 8];
  k *= get_monster_exp(mlevel, tier);
  k /= 16;

  return k;
}

const uint8_t agl_mod[4] = { 2, 4, 8, 12 };
const uint16_t atk_def_mod[4] = { 1, 2, 4, 6 };

bool roll_flee(uint8_t agl, uint8_t block_agl) BANKED {
  if (block_agl > agl + 10)
    return false;
  if (agl > block_agl + 10)
    return true;
  return rand() < 128;
}

uint8_t agl_down(uint8_t base_agl, PowerTier tier) BANKED {
  const uint8_t mod = agl_mod[tier];
  return base_agl < mod ? 0 : base_agl - mod;
}

uint8_t atk_down(uint8_t base, PowerTier tier) BANKED {
  uint16_t k = atk_def_mod[tier];
  k *= base;
  k >>= 4;
  k = base - k;
  return k;
}

uint8_t def_down(uint8_t base, PowerTier tier) BANKED {
  uint16_t k = atk_def_mod[tier];
  k *= base;
  k >>= 4;
  k = base - k;
  return k;
}

uint8_t agl_up(uint8_t base_agl, PowerTier tier) BANKED {
  return base_agl + agl_mod[tier];
}

uint8_t atk_up(uint8_t base, PowerTier tier) BANKED {
  uint16_t k = atk_def_mod[tier];
  k *= base;
  k >>= 4;
  k += base;
  return k;
}

uint8_t def_up(uint8_t base, PowerTier tier) BANKED {
  uint16_t k = atk_def_mod[tier];
  k *= base;
  k >>= 4;
  k += base;
  return k;
}

bool fear_flee_roll(PowerTier tier) BANKED {
  const uint8_t chance_tbl[4] = { 64, 96, 135, 185 };
  return d256() < chance_tbl[tier < 4 ? tier : 0];
}

bool fear_shiver_roll(PowerTier tier) BANKED {
  const uint8_t chance_tbl[4] = { 128, 135, 185, 240 };
  return d256() < chance_tbl[tier < 4 ? tier : 0];
}

bool paralyzed_roll(PowerTier tier) BANKED {
  const uint8_t chance_tbl[4] = { 192, 212, 230, 250 };
  return d256() < chance_tbl[tier < 4 ? tier : 0];
}

uint16_t poison_hp(PowerTier tier, uint16_t max_hp) BANKED {
  uint16_t k = max_hp * (tier < 4 ? tier + 2 : 1);
  k >>= 4;
  return k;
}

bool confused_attack(PowerTier tier) BANKED {
  const uint8_t chance_tbl[4] = { 64, 96, 128, 192 };
  return d256() < chance_tbl[tier < 4 ? tier : 0];
}

uint16_t regen_hp(PowerTier tier, uint16_t max_hp) BANKED {
  uint16_t k = max_hp * (tier < 4 ? tier + 2 : 1);
  k >>= 4;
  return k;
}
