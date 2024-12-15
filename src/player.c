#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdbool.h>
#include <stdio.h>

#include "battle.h"
#include "encounter.h"
#include "player.h"
#include "monster.h"
#include "tables.h"
#include "strings.h"
#include "util.h"

Player player = {
  CLASS_DRUID,
};

void init_player(void) {
  player.message_speed = 3;

  player.level = 10;
  player.exp = get_exp(10) + 23;
  player.next_level_exp = get_exp(11);

  player.hp = player.max_hp = get_player_hp(10, B_TIER);
  player.sp = player.max_sp = get_player_sp(10, A_TIER);
  player.atk = player.atk_base = get_player_atk(10, C_TIER);
  player.def = player.def_base = get_player_def(10, B_TIER);
  player.matk = player.matk_base = get_player_atk(10, A_TIER);
  player.mdef = player.mdef_base = get_player_def(17, B_TIER);
  player.summon = &summon_kobold;
  player.default_summon = &summon_kobold;
}

void reset_player_stats(void) {
  player.atk = player.atk_base;
  player.def = player.def_base;
  player.matk = player.matk_base;
  player.mdef = player.mdef_base;
}

/**
 * Applies damage to a monster taking vulnerabilities, immunites, and
 * resistances into account.
 * @param monster The monster to damage.
 * @param base_damage Base damage for the damage roll.
 * @param type The damage type.
 */
void damage_monster(
  MonsterInstance *monster,
  uint16_t base_damage,
  DamageType type
) {
  if (monster->aspect_immune & type) {
    sprintf(battle_post_message, str_battle_player_hit_immune);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  monster->target_hp = monster->hp < damage ? 0 : monster->hp - damage;
  bool critical = roll >= 12;

  if (critical) {
    sprintf(battle_post_message, str_battle_player_hit_crit, damage);
  } else  if (monster->aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_battle_player_hit_resist, damage);
  } else if (monster->aspect_vuln & type) {
    damage <<= 1;
  } else {
    sprintf(battle_post_message, str_battle_player_hit, damage);
  }
}

//------------------------------------------------------------------------------
// Summon 1 - Wilbur the Kobold
//------------------------------------------------------------------------------

/**
 * Activation method for the kobold.
 * @see `Summon.activate`
 */
void kobold_activate(void) {
  switch (player.player_class) {
  case CLASS_DRUID:
    summon_kobold.ability = &kobold_vine_whip;
    break;
  // case CLASS_FIGHTER:
  //   break;
  // case CLASS_MONK:
  //   break;
  // case CLASS_SORCERER:
  //   break;
  }
}


/**
 * Base attack ability execution function for the kobold.
 * @param target Monster instance target for the attack.
 */
void kobold_base_attack(MonsterInstance *target) {
  const char *msg = is_magic_class() ?
    str_battle_kobold_base_magic_attack :
    str_battle_kobold_base_attack;
  sprintf(battle_pre_message, msg);

  const uint8_t atk = is_magic_class() ? player.matk : player.atk;
  if (!roll_attack(atk, target->def)) {
    sprintf(battle_post_message, str_battle_player_miss);
    return;
  }

  const uint8_t def = is_magic_class() ? target->mdef : target->def;
  const uint8_t level = level_offset(player.level, -2);
  const uint16_t base_dmg = get_player_damage(level, B_TIER);
  damage_monster(target, base_dmg, DAMAGE_PHYSICAL);
}

Summon summon_kobold = {
  1,
  str_summon_name_kobold,
  str_summon_name_wilbur,
  B_TIER,
  EMPTY_BITFIELD,
  DAMAGE_EARTH,
  DAMAGE_LIGHT,
  EMPTY_BITFIELD,
  kobold_activate,
  kobold_base_attack,
};

void execute_kobold_vine_whip(void) {
}

Ability kobold_vine_whip = {
  str_ability_vine_whip,
  CLASS_DRUID,
  C_TIER,
  3,
  TARGET_SINGLE,
  2,
  { FONT_SPACE, FONT_SPACE, FONT_DIGIT(2) },
  execute_kobold_vine_whip,
  &kobold_mend // next
};

void execute_kobold_mend(void) {
}

Ability kobold_mend = {
  str_ability_mend,
  CLASS_DRUID,
  C_TIER,
  5,
  TARGET_SELF,
  5,
  { FONT_SPACE, FONT_SPACE, FONT_DIGIT(5) },
  execute_kobold_mend,
  &kobold_summon_ants,
};

void execute_kobold_summon_ants(void) {
}

Ability kobold_summon_ants = {
  str_ability_summon_ants,
  CLASS_DRUID,
  B_TIER,
  9,
  TARGET_SINGLE,
  9,
  { FONT_SPACE, FONT_SPACE, FONT_DIGIT(9) },
  execute_kobold_summon_ants,
  &kobold_dirty_fang,
};

void execute_kobold_dirty_fang(void) {
}

Ability kobold_dirty_fang = {
  str_ability_dirty_fang,
  CLASS_DRUID,
  B_TIER,
  14,
  TARGET_SINGLE,
  12,
  { FONT_SPACE, FONT_DIGIT(1), FONT_DIGIT(2) },
  execute_kobold_dirty_fang,
  NULL
};
