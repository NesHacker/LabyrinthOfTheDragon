#pragma bank 4

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

Player player = { "", CLASS_DRUID };
const Ability *class_abilities[6];
const Ability *player_abilities[6];
uint8_t player_num_abilities = 0;

const Ability null_ability = { 0 };

void ability_placeholder(void) {
  sprintf(battle_pre_message, "You try a thing.");
  sprintf(battle_post_message, "It doesn't work.");
}

/**
 * Updates player stats based on the given power tiers.
 * @param hp Power tier for hp.
 * @param sp Power tier for sp.
 * @param atk Power tier for atk.
 * @param def Power tier for def.
 * @param matk Power tier for matk.
 * @param mdef Power tier for mdef.
 * @param agl Power tier for agl.
 */
void update_stats(
  PowerTier hp,
  PowerTier sp,
  PowerTier atk,
  PowerTier def,
  PowerTier matk,
  PowerTier mdef,
  PowerTier agl
) {
  player.max_hp = get_player_hp(player.level, hp);
  player.max_sp = get_player_sp(player.level, sp);
  player.atk_base = get_player_atk(player.level, atk);
  player.def_base = get_player_def(player.level, def);
  player.matk_base = get_player_atk(player.level, matk);
  player.mdef_base = get_player_def(player.level, mdef);
  player.agl_base = get_agl(player.level, agl);
}

/**
 * Applies damage to the target monster. Takes immunities, etc. into account and
 * handles battle result messages.
 * @param base_damage Base damage for the attack.
 * @param type Aspect for the damage.
 */
void damage_monster(uint16_t base_damage, DamageAspect type) {
  Monster *monster = encounter.target;

  if (!monster)
    return;

  if (monster->aspect_immune & type) {
    sprintf(battle_post_message, str_battle_player_hit_immune);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = is_critical(roll);

  if (critical) {
    sprintf(battle_post_message, str_battle_player_hit_crit, damage);
  } else if (monster->aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_battle_player_hit_resist, damage);
  } else if (monster->aspect_vuln & type) {
    damage <<= 1;
  } else {
    sprintf(battle_post_message, str_battle_player_hit, damage);
  }

  if (monster->target_hp < damage)
    monster->target_hp = 0;
  else
    monster->target_hp -= damage;
}

/**
 * Applies damage to all active monsters in the encounter. Takes immmunities,
 * etc. into account. Does **NOT** handle battle result messages.
 * @param base_damage Base damage for the attack.
 * @param atk ATK of the attacker.
 * @param use_mdef Whether or not to use DEF or MDEF when checking attack roll.
 * @param type Aspect type for the damage.
 * @return Number of monsters hit by the attack.
 */
uint8_t damage_all(
  uint8_t base_damage,
  uint8_t atk,
  bool use_mdef,
  DamageAspect type
) {
  uint8_t dam_roll = d16();
  uint16_t damage = calc_damage(dam_roll, base_damage);

  Monster *monster = encounter.monsters;
  uint8_t atk_roll = d256();
  uint8_t hits = 0;

  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;
    if (monster->aspect_immune & type)
      continue;

    const uint8_t def = use_mdef ? monster->mdef : monster->def;
    if (!check_attack(atk_roll, atk, def))
      continue;

    hits++;

    uint16_t d = damage;
    if (monster->aspect_resist & type) {
      d = damage >> 1;
    } else if (monster->aspect_vuln & type) {
      d = damage << 1;
    }

    if (monster->target_hp < d)
      monster->target_hp = 0;
    else
      monster->target_hp -= d;
  }

  return hits;
}

//------------------------------------------------------------------------------
// Class: Druid
//------------------------------------------------------------------------------

void druid_update_stats(void) {
  update_stats(
    B_TIER,
    B_TIER,
    C_TIER,
    B_TIER,
    B_TIER,
    A_TIER,
    B_TIER
  );
}

void druid_base_attack(void) {
  sprintf(battle_pre_message, str_battle_poison_spray);

  Monster *target = encounter.target;
  if (!roll_attack(player.matk, target->mdef)) {
    sprintf(battle_post_message, str_battle_player_miss);
    return;
  }

  PowerTier damage_tier;
  if (player.level < 35)
    damage_tier = C_TIER;
  else if (player.level < 75)
    damage_tier = B_TIER;
  else
    damage_tier = A_TIER;

  const uint16_t base_dmg = get_player_damage(player.level, damage_tier);
  damage_monster(base_dmg, DAMAGE_MAGICAL);
}

void druid_cure_wounds(void) {
  sprintf(battle_pre_message, str_battle_cure_wounds);
  const uint16_t base_hp = get_player_heal(player.level, C_TIER);
  const uint8_t roll = d16();
  const uint16_t hp = calc_damage(roll, base_hp);

  heal_player(hp);

  if (is_critical(roll))
    sprintf(battle_post_message, str_battle_player_heal_crit, hp);
  else if (is_fumble(roll))
    sprintf(battle_post_message, str_battle_player_heal_fumble, hp);
  else
    sprintf(battle_post_message, str_battle_player_heal, hp);
}

void druid_bark_skin(void) {
  ability_placeholder();
}

void druid_lightning(void) {
  ability_placeholder();
}

void druid_heal(void) {
  ability_placeholder();
}

void druid_insect_plague(void) {
  ability_placeholder();
}

void druid_regen(void) {
  ability_placeholder();
}


//------------------------------------------------------------------------------
// Class: Fighter
//------------------------------------------------------------------------------

void fighter_update_stats(void) {
  update_stats(
    A_TIER,
    C_TIER,
    B_TIER,
    A_TIER,
    C_TIER,
    B_TIER,
    B_TIER
  );
}

void fighter_base_attack(void) {
  // Sword
}

//------------------------------------------------------------------------------
// Class: Monk
//------------------------------------------------------------------------------

void monk_update_stats(void) {
  update_stats(
    B_TIER,
    B_TIER,
    B_TIER,
    B_TIER,
    C_TIER,
    B_TIER,
    A_TIER
  );
}

void monk_base_attack(void) {
  // Fist
}

//------------------------------------------------------------------------------
// Class: Sorcerer
//------------------------------------------------------------------------------

void sorcerer_update_stats(void) {
  update_stats(
    C_TIER,
    A_TIER,
    C_TIER,
    C_TIER,
    A_TIER,
    B_TIER,
    A_TIER
  );
}

void sorcerer_base_attack(void) {
  // Fist
}

//------------------------------------------------------------------------------
// Class: Test Class
//------------------------------------------------------------------------------

void test_class_update_stats(void) {
  update_stats(
    S_TIER,
    S_TIER,
    S_TIER,
    S_TIER,
    S_TIER,
    S_TIER,
    S_TIER
  );
}

void test_class_base_attack(void) {
  ability_placeholder();
}

void test_class_ability0(void) {
  sprintf(battle_pre_message, "Attacking all\x60");
  const uint8_t base_damage = get_player_damage(player.level, B_TIER);
  const uint8_t hits = damage_all(base_damage, player.atk, false, DAMAGE_PHYSICAL);

  if (!hits) {
    sprintf(battle_post_message, str_battle_player_miss_all);
  } else {
    sprintf(battle_post_message, "Damage Applied.");
  }
}

void test_class_ability1(void) {
  sprintf(battle_pre_message, "(De)buffing\x60");
  skip_post_message = true;

  // player.hp = 1;
  // player.sp = 1;

  apply_poison(encounter.player_status_effects, C_TIER, 10, 0);
  apply_def_down(encounter.player_status_effects, C_TIER, 10, 0);
  apply_atk_down(encounter.player_status_effects, C_TIER, 10, 0);
  apply_agl_down(encounter.player_status_effects, C_TIER, 10, 0);

  // player.hp = 1;

  // Monster *monster = encounter.monsters;
  // for (uint8_t k = 0; k < 3; k++, monster++) {
  //   if (!monster->active)
  //     continue;
  //   StatusEffectInstance *effects = monster->status_effects;
  //   apply_regen(effects, C_TIER, 6, monster->debuff_immune);
  // }
}

void test_class_ability2(void) {
  sprintf(battle_pre_message, "SUPERKILL!!!");
  skip_post_message = true;
  damage_all(5000, 200, false, DAMAGE_PHYSICAL);
}

void test_class_ability3(void) {
  ability_placeholder();
}

void test_class_ability4(void) {
  ability_placeholder();
}

void test_class_ability5(void) {
  ability_placeholder();
}

//------------------------------------------------------------------------------
// Common functions
//------------------------------------------------------------------------------

/**
 * Updates a player's stats based on their current class and level.
 */
void update_player_stats(void) {
  switch (player.player_class) {
  case CLASS_DRUID:
    druid_update_stats();
    break;
  case CLASS_FIGHTER:
    fighter_update_stats();
    break;
  case CLASS_MONK:
    monk_update_stats();
    break;
  case CLASS_SORCERER:
    sorcerer_update_stats();
    break;
  case CLASS_TEST:
    test_class_update_stats();
    break;
  }
}

/**
 * Sets class abilities based on the current player class.
 */
void set_class_abilities(void) {
  switch (player.player_class) {
  case CLASS_DRUID:
    class_abilities[0] = &druid0;
    class_abilities[1] = &druid1;
    class_abilities[2] = &druid2;
    class_abilities[3] = &druid3;
    class_abilities[4] = &druid4;
    class_abilities[5] = &druid5;
    break;
  case CLASS_FIGHTER:
    class_abilities[0] = &fighter0;
    class_abilities[1] = &fighter1;
    class_abilities[2] = &fighter2;
    class_abilities[3] = &fighter3;
    class_abilities[4] = &fighter4;
    class_abilities[5] = &fighter5;
    break;
  case CLASS_MONK:
    class_abilities[0] = &monk0;
    class_abilities[1] = &monk1;
    class_abilities[2] = &monk2;
    class_abilities[3] = &monk3;
    class_abilities[4] = &monk4;
    class_abilities[5] = &monk5;
    break;
  case CLASS_SORCERER:
    class_abilities[0] = &sorcerer0;
    class_abilities[1] = &sorcerer1;
    class_abilities[2] = &sorcerer2;
    class_abilities[3] = &sorcerer3;
    class_abilities[4] = &sorcerer4;
    class_abilities[5] = &sorcerer5;
    break;
  case CLASS_TEST:
    class_abilities[0] = &test_class0;
    class_abilities[1] = &test_class1;
    class_abilities[2] = &test_class2;
    class_abilities[3] = &test_class3;
    class_abilities[4] = &test_class4;
    class_abilities[5] = &test_class5;
    break;
  }
}

/**
 * Sets abilities based on the player's current class.
 */
void set_player_abilities(void) {
  uint8_t flags = player.ability_flags;
  player_num_abilities = 0;

  for (uint8_t k = 0; k < MAX_ABILITIES; k++, flags >>= 1) {
    player_abilities[player_num_abilities] = &null_ability;
    if (flags & 1)
      player_abilities[player_num_abilities++] = class_abilities[k];
  }
}

// -----------------------------------------------------------------------------

void grant_ability(AbilityFlag flag) BANKED {
  player.ability_flags |= flag;
  set_player_abilities();
}

void set_player_level(uint8_t level) BANKED {
  player.level = level;
  player.exp = get_exp(player.level);
  player.next_level_exp = get_exp(player.level + 1);
  update_player_stats();
  full_heal_player();
}

void init_player(PlayerClass player_class) BANKED {
  player.player_class = player_class;
  set_class_abilities();
  player.ability_flags = 0;
  grant_ability(ABILITY_0);
  set_player_level(1);
  reset_player_stats();

  // TODO Move these assignments into the character creator
  sprintf(player.name, "Hero");
  player.message_speed = AUTO_PAGE_MED;

}

bool level_up(uint16_t xp) BANKED {
  bool level_up = false;
  player.exp += xp;

  while (player.exp > player.next_level_exp) {
    level_up = true;
    player.level++;
    player.next_level_exp = get_exp(player.level + 1);
  }

  if (level_up) {
    update_player_stats();
    full_heal_player();
  }

  return level_up;
}

void player_base_attack(void) BANKED {
  switch (player.player_class) {
  case CLASS_DRUID:
    druid_base_attack();
    break;
  case CLASS_FIGHTER:
    fighter_base_attack();
    break;
  case CLASS_MONK:
    monk_base_attack();
    break;
  case CLASS_SORCERER:
    sorcerer_base_attack();
    break;
  case CLASS_TEST:
    test_class_base_attack();
    break;
  }
}

void player_use_ability(const Ability *ability) BANKED {
  ability->execute();
}
