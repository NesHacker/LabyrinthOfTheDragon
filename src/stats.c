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

// TODO mod me
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

bool roll_attack(uint8_t atk, uint8_t def) BANKED {
  if (atk + 32 < def)
    return rand() < attack_roll_target[0];
  if (atk > 32 + 32)
    return rand() < attack_roll_target[64];
  return rand() < attack_roll_target[atk - def + 32];
}

bool check_attack(uint8_t d256_roll, uint8_t atk, uint8_t def) BANKED {
  if (atk + 32 < def)
    return d256_roll < attack_roll_target[0];
  if (atk > 32 + 32)
    return d256_roll < attack_roll_target[64];
  return d256_roll < attack_roll_target[atk - def + 32];
}

uint16_t calc_damage(uint8_t d16_roll, uint16_t base_dmg) BANKED {
  return (damage_roll_modifier[d16_roll & 0x0F] * base_dmg) >> 4;
}

uint16_t calc_monster_exp(uint8_t level, PowerTier tier) BANKED {
  // Monster is eight levels or more below the player's level.
  if (level + 8 <= player.level)
    return 0;

  // Monster seven levels or higher tha n the player's level.
  if (level >= player.level + 7)
    return get_monster_exp(level, tier) << 1;

  // Otherwise use the experience adjust calculator
  const uint8_t index = player.level - level;
  const uint16_t multipler = xp_mod[index];
  const uint16_t exp = get_monster_exp(level, tier);
  return (multipler * exp) >> 4;
}

// TODO Move these out into tables?
const uint8_t blind_mod[4] = { 8, 12, 16, 22 };
const uint8_t agl_mod[4] = { 2, 4, 8, 12 };
const uint16_t atk_def_mod[4] = { 1, 2, 4, 6 };

bool roll_flee(uint8_t agl, uint8_t block_agl) BANKED {
  // TODO Flesh this out with a table
  if (block_agl > agl + 10)
    return false;
  if (agl > block_agl + 10)
    return true;
  return rand() < 128;
}

uint8_t blind_atk(uint8_t base_atk, PowerTier tier) BANKED {
  const uint8_t mod = blind_mod[tier];
  return base_atk < mod ? 0 : base_atk - mod;
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
