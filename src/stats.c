#pragma bank 5

#include "stats.h"

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

bool roll_attack(uint8_t atk, uint8_t def) BANKED {
  if (atk + 32 < def)
    return rand() < attack_roll_target[0];
  if (atk > 32 + 32)
    return rand() < attack_roll_target[64];
  return rand() < attack_roll_target[atk - def + 32];
}

uint16_t calc_damage(uint8_t d16_roll, uint16_t base_dmg) BANKED {
  return (damage_roll_modifier[d16_roll & 0x0F] * base_dmg) >> 4;
}

uint16_t calc_monster_exp(uint8_t plvl, uint8_t mlvl, PowerTier mtier) BANKED {
  if (plvl - mlvl + 8 <= 0)
    return 0;
  if (mlvl - plvl + 8 >= 15)
    return get_monster_exp(mlvl, mtier) << 1;
  return (xp_mod[plvl - mlvl + 8] * get_monster_exp(mlvl, mtier)) >> 4;
}
