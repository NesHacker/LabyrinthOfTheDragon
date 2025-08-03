#pragma bank 4

#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdbool.h>
#include <stdio.h>

#include "battle.h"
#include "battle.effects.h"
#include "encounter.h"
#include "player.h"
#include "monster.h"
#include "tables.h"
#include "sound.h"
#include "strings.h"

Player player = { "", CLASS_TEST };
const Ability *class_abilities[6];
const Ability *player_abilities[6];
uint8_t player_num_abilities = 0;

const Ability null_ability = { 0 };

/**
 * Used as a placeholder for abiltiies prior to implementation.
 */
static void ability_placeholder(void) {
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
static void update_stats(
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
static void damage_monster(uint16_t base_damage, DamageAspect type) {
  Monster *monster = encounter.target;

  if (!monster)
    return;

  if (monster->aspect_immune & type) {
    sprintf(battle_post_message, str_player_hit_immune);
    return;
  }

  switch (monster->type) {
  case MONSTER_DISPLACER_BEAST:
    monster->parameter--;
    if (monster->parameter != 0)
      break;
    monster->parameter = monster->exp_tier > B_TIER ? 2 : 4;
    sprintf(battle_post_message, str_player_displacer_beast_phase);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = is_critical(roll);
  bool hasted = has_special(SPECIAL_HASTE);

  if (hasted)
    damage += calc_damage(d16(), base_damage);

  if (critical) {
    sprintf(battle_post_message, str_player_hit_crit, damage);
  } else if (monster->aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_player_hit_resist, damage);
  } else if (monster->aspect_vuln & type) {
    damage <<= 1;
    sprintf(battle_post_message, str_player_hit_vuln, damage);
  } else {
    sprintf(battle_post_message, str_player_hit, damage);
  }

  if (monster->target_hp < damage) {
    monster->target_hp = 0;
    if (
      monster->type == MONSTER_DEATHKNIGHT &&
      !(monster->parameter & DEATH_KNIGHT_REVIVE_USED) &&
      d16() < 3
    ) {
      monster->parameter |= DEATH_KNIGHT_REVIVE_USED;
      monster->target_hp = monster->max_hp / 4;
      sprintf(battle_post_message, str_player_deathknight_revive);
    }
  }
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
static uint8_t damage_all(
  uint16_t base_damage,
  uint8_t atk,
  bool use_mdef,
  DamageAspect type
) {
  uint8_t dam_roll = d16();
  uint16_t damage = calc_damage(dam_roll, base_damage);

  if (has_special(SPECIAL_HASTE))
    damage += calc_damage(d16(), base_damage);

  Monster *monster = encounter.monsters;
  uint8_t atk_roll = d256();
  uint8_t hits = 0;

  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;
    if (monster->aspect_immune & type)
      continue;

    bool evaded = false;

    switch (monster->type) {
    case MONSTER_DISPLACER_BEAST:
      monster->parameter--;
      if (monster->parameter != 0)
        break;
      monster->parameter = monster->exp_tier > B_TIER ? 2 : 4;
      evaded = true;
      break;
    }

    if (evaded)
      continue;

    const uint8_t def = use_mdef ? monster->mdef : monster->def;
    if (!check_attack(atk_roll, attack_roll_player, atk, def))
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

/**
 * Applies damage to all active monsters in the encounter. Cannot miss, but
 * takes immunities and resistances into account.
 * @param base_damage Base damage for the attack.
 * @param type Aspect type for the damage.
 */
static void damage_all_no_miss(uint16_t base_damage, DamageAspect type) {
  uint8_t dam_roll = d16();
  uint16_t damage = calc_damage(dam_roll, base_damage);

  if (has_special(SPECIAL_HASTE))
    damage += calc_damage(d16(), base_damage);

  Monster *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;

    if (monster->aspect_immune & type)
      continue;

    bool evaded = false;
    switch (monster->type) {
    case MONSTER_DISPLACER_BEAST:
      monster->parameter--;
      if (monster->parameter != 0)
        break;
      monster->parameter = monster->exp_tier > B_TIER ? 2 : 4;
      evaded = true;
      break;
    }

    if (evaded)
      continue;

    uint16_t d = damage;
    if (monster->aspect_resist & type)
      d = damage >> 1;
    else if (monster->aspect_vuln & type)
      d = damage << 1;

    if (monster->target_hp < d)
      monster->target_hp = 0;
    else
      monster->target_hp -= d;
  }
}

/**
 * Heals the player without going over max HP.
 * @param hp Amount of HP to heal the player.
 * @param msg_format Message format to use for the battle text.
 */
uint16_t heal_player(uint16_t hp) {
  if (has_special(SPECIAL_HASTE))
    hp = (hp * 3) / 2;
  if (player.hp + hp > player.max_hp)
    hp = player.max_hp - player.hp;
  player.hp += hp;
  PLAYER_HEAL(hp);
  return hp;
}

//------------------------------------------------------------------------------
// Class: Druid
//------------------------------------------------------------------------------

static void druid_update_stats(void) {
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
  sprintf(battle_pre_message, str_player_poison_spray);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.matk, target->mdef)) {
    PLAYER_MISS;
    return;
  }

  PowerTier damage_tier = C_TIER;

  const uint16_t base_dmg = get_player_damage(player.level, damage_tier);
  damage_monster(base_dmg, DAMAGE_MAGICAL);
  SFX_POISON_SPRAY;
}

void druid_cure_wounds(void) {
  sprintf(battle_pre_message, str_player_cure_wounds);
  heal_player(player.max_hp / 2);
}

void druid_bark_skin(void) {
  sprintf(battle_pre_message, str_player_bark_skin);
  SKIP_POST_MSG;
  apply_def_up(encounter.player_status_effects, B_TIER, 0);
  apply_special(SPECIAL_BARKSKIN);
}

void druid_lightning(void) {
  sprintf(battle_pre_message, str_player_lightning);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.matk, target->mdef)) {
    PLAYER_MISS;
    return;
  }

  PowerTier damage_tier = A_TIER;

  const uint16_t base_dmg = get_player_damage(
    level_offset(player.level, 10), damage_tier);

  damage_monster(base_dmg, DAMAGE_AIR);
}

void druid_heal(void) {
  sprintf(battle_pre_message, str_player_heal);
  heal_player(player.max_hp);
}

void druid_insect_plague(void) {
  sprintf(battle_pre_message, str_player_insect_plague);

  PowerTier tier = B_TIER;
  if (player.level > 75)
    tier = S_TIER;
  else if (player.level > 35)
    tier = A_TIER;

  const uint8_t level = level_offset(player.level, 5);
  const uint16_t base_damage = get_player_damage(level, tier);
  uint8_t hits = damage_all(base_damage, player.matk, true, DAMAGE_MAGICAL);

  if (hits == 0)
    PLAYER_MISS_ALL;
  else
    SKIP_POST_MSG;
}

void druid_regen(void) {
  sprintf(battle_pre_message, str_player_regen);
  SKIP_POST_MSG;
  PowerTier tier = player.level > 75 ? S_TIER : A_TIER;
  apply_regen(encounter.player_status_effects, tier, 0);
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
  sprintf(battle_pre_message, str_player_fighter_attack);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.atk, target->def)) {
    PLAYER_MISS;
    return;
  }

  damage_monster(get_player_damage(player.level, C_TIER), DAMAGE_PHYSICAL);
  battle_sfx = sfx_melee_attack;
}

void fighter_second_wind(void) {
  sprintf(battle_pre_message, str_player_second_wind);
  heal_player(player.max_hp / 4);
}

void fighter_action_surge(void) {
  sprintf(battle_pre_message, str_player_action_surge);

  PowerTier tier = B_TIER;
  if (player.level > 20)
    tier = A_TIER;
  if (player.level > 40)
    tier = S_TIER;

  Monster *target = encounter.target;
  const uint8_t attack_level = level_offset(player.level, 3);
  uint16_t base_dmg = get_player_damage(attack_level, tier);
  damage_monster(base_dmg * 2, DAMAGE_PHYSICAL);
}

void fighter_cleave(void) {
  sprintf(battle_pre_message, str_player_cleave);

  PowerTier tier = C_TIER;
  if (player.level > 20)
    tier = B_TIER;
  if (player.level > 50)
    tier = A_TIER;
  if (player.level > 75)
    tier = S_TIER;

  const uint8_t level = level_offset(player.level, -2);
  const uint16_t base_damage = get_player_damage(level, tier);
  uint8_t hits = damage_all(base_damage, player.matk, true, DAMAGE_MAGICAL);

  if (hits == 0)
    PLAYER_MISS_ALL;
  else
    SKIP_POST_MSG;
}

void fighter_trip_attack(void) {
  sprintf(battle_pre_message, str_player_trip_attack);

  Monster *target = encounter.target;
  uint8_t def = get_monster_def(level_offset(target->level, -5), C_TIER);
  if (!roll_attack_player(player.atk, def)) {
    PLAYER_MISS;
    return;
  }

  uint8_t turns = 2;
  if (player.level > 60)
    turns = 4;
  else if (player.level > 30)
    turns = 3;

  target->trip_turns = turns;
  sprintf(battle_post_message, str_player_trip_attack_hit);
}

void fighter_menace(void) {
  sprintf(battle_pre_message, str_player_menace);
  SKIP_POST_MSG;

  PowerTier tier = A_TIER;
  uint8_t turns = 2;
  if (player.level > 30)
    turns = 3;
  if (player.level > 60) {
    turns = 4;
    tier = S_TIER;
  }

  Monster *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;
    apply_scared(monster->status_effects, tier, turns, monster->debuff_immune);
  }
}

void fighter_indomitable(void) {
  sprintf(battle_pre_message, str_player_indomitable);
  SKIP_POST_MSG;
  player.aspect_resist = 0xFF;
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
  sprintf(battle_pre_message, str_player_monk_attack);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.atk + player.agl, target->def)) {
    PLAYER_MISS;
    return;
  }

  const uint16_t base_dmg = get_player_damage(player.level, B_TIER);
  damage_monster(base_dmg, DAMAGE_PHYSICAL);
  SFX_MONK_STRIKE;
}

void monk_evasion(void) {
  sprintf(battle_pre_message, str_player_monk_evasion);
  SKIP_POST_MSG;

  player.special_flags |= SPECIAL_EVASION;

  PowerTier agl_up_tier = C_TIER;
  uint8_t agl_up_duration = 2;

  if (player.level > 35) {
    agl_up_tier = B_TIER;
    agl_up_duration = 3;
  }

  if (player.level > 70) {
    agl_up_tier = A_TIER;
  }

  apply_agl_up(encounter.player_status_effects, agl_up_tier, agl_up_duration);
  SFX_MID_POWERUP;
}

void monk_open_palm(void) {
  Monster *target = encounter.target;
  uint8_t atk = player.atk + player.agl;

  if (!roll_attack_player(atk, target->def)) {
    sprintf(battle_pre_message, str_player_monk_open_palm);
    PLAYER_MISS;
    return;
  }

  PowerTier damage_tier = B_TIER;
  if (player.level >= 65)
    damage_tier = S_TIER;
  else if (player.level >= 30)
    damage_tier = B_TIER;

  uint8_t trip_chance = 2;
  if (player.level >= 65)
    trip_chance = 4;
  else if (player.level > 30)
    trip_chance = 3;

  if (d8() < trip_chance && !(target->special_immune & SPECIAL_SLEET_STORM)) {
    encounter.target->trip_turns = player.level > 30 ? 3 : 2;
    sprintf(battle_pre_message, str_player_monk_open_palm_trip,
      encounter.target->name, encounter.target->id);
  } else {
    sprintf(battle_pre_message, str_player_monk_open_palm);
  }

  uint8_t attack_level = level_offset(player.level, player.agl);
  const uint16_t base_dmg = get_player_damage(attack_level, damage_tier);
  damage_monster(base_dmg, DAMAGE_PHYSICAL);
}

void monk_still_mind(void) {
  StatusEffectInstance *effect = encounter.player_status_effects;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
    if (!effect->active)
      continue;
    if (is_debuff(effect->effect))
      effect->active = false;
  }
  sprintf(battle_pre_message, str_player_monk_still_mind);
  sprintf(battle_post_message, str_player_monk_still_mind_post);
}

void monk_flurry(void) {
  sprintf(battle_pre_message, str_player_monk_flurry_of_blows);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.atk + player.agl, target->def)) {
    PLAYER_MISS;
    return;
  }

  uint8_t attacks = 2;
  if (player.level > 80)
    attacks = 4;
  if (player.level > 60)
    attacks = 3;

  PowerTier damage_tier = B_TIER;
  if (player.level >= 65)
    damage_tier = S_TIER;
  else if (player.level >= 30)
    damage_tier = A_TIER;

  uint8_t attack_level = level_offset(player.level, player.agl);
  uint16_t base_dmg = get_player_damage(attack_level, damage_tier);
  base_dmg *= attacks;

  damage_monster(base_dmg, DAMAGE_PHYSICAL);
}

void monk_diamond_body(void) {
  sprintf(battle_pre_message, str_player_monk_diamond_body);
  SKIP_POST_MSG;

  PowerTier def_up_tier = B_TIER;
  if (player.level > 50)
    def_up_tier = A_TIER;

  player.aspect_resist = DAMAGE_PHYSICAL | DAMAGE_MAGICAL;
  apply_def_up(encounter.player_status_effects, def_up_tier, 0);
}

void monk_quivering_palm(void) {
  sprintf(battle_pre_message, str_player_monk_quivering_palm);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.atk + player.agl, target->def)) {
    PLAYER_MISS;
    return;
  }

  if (!(target->special_immune & SPECIAL_INSTANT_KILL)) {
    uint8_t kill_chance = 1;
    if (player.level > 60)
      kill_chance = 2;
    if (player.level > 80)
      kill_chance = 3;

    if (d8() < kill_chance) {
      sprintf(battle_post_message, str_player_monk_quivering_kill);
      encounter.target->target_hp = 0;
      return;
    }
  }

  uint8_t attack_level = level_offset(player.level, player.agl);
  uint16_t base_dmg = get_player_damage(attack_level, S_TIER);
  base_dmg *= 2;

  damage_monster(base_dmg, DAMAGE_PHYSICAL);
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
  sprintf(battle_pre_message, str_player_sorc_magic_missile_one);
  const PowerTier tier = B_TIER;
  const uint8_t level = level_offset(player.level, 1);
  damage_monster(get_player_damage(level, tier), DAMAGE_MAGICAL);
  SFX_MAGIC_MISSILE;
}

void sorcerer_darkness(void) {
  sprintf(battle_pre_message, str_player_sorc_darkness);
  SKIP_POST_MSG;

  uint8_t turns = 3;
  if (player.level >= 30)
    turns = 4;
  if (player.level >= 50)
    turns = 5;

  PowerTier tier = B_TIER;
  if (player.level >= 45)
    tier = A_TIER;

  Monster *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;
    apply_blind(monster->status_effects, tier, turns, monster->debuff_immune);
  }

  SFX_MAGIC;
}

void sorcerer_fireball(void) {
  sprintf(battle_pre_message, str_player_sorc_fireball);
  SKIP_POST_MSG;

  Monster *monster = encounter.monsters;
  uint8_t mdef = monster->mdef;

  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;
    if (monster->mdef < mdef)
      mdef = monster->mdef;
  }

  PowerTier tier = C_TIER;
  if (player.level >= 50)
    tier = B_TIER;

  uint16_t damage = get_player_damage(player.level, tier);

  if (!roll_attack_player(player.matk, mdef))
    damage /= 2;

  damage_all_no_miss(damage, DAMAGE_FIRE);
}

void sorcerer_haste(void) {
  sprintf(battle_pre_message, str_player_sorc_haste);
  SKIP_POST_MSG;
  apply_haste(encounter.player_status_effects, B_TIER, 0);
}

void sorcerer_sleetstorm(void) {
  sprintf(battle_pre_message, str_player_sorc_sleetstorm);
  SKIP_POST_MSG;
  player.special_flags |= SPECIAL_SLEET_STORM;
}

void sorcerer_disintegrate(void) {
  sprintf(battle_pre_message, str_player_sorc_disintegrate);

  Monster *target = encounter.target;
  if (!roll_attack_player(player.matk + 10, target->mdef)) {
    PLAYER_MISS;
    return;
  }

  Monster *monster = encounter.target;
  if(!(monster->special_immune & SPECIAL_INSTANT_KILL)) {
    uint8_t kill_chance = 2;
    if (player.level > 40)
      kill_chance = 3;
    if (player.level > 60)
      kill_chance = 4;

    if (d8() < kill_chance) {
      sprintf(battle_post_message, str_player_sorc_disintegrate_kill);
      encounter.target->target_hp = 0;
      return;
    }
  }

  uint8_t attack_level = level_offset(player.level, 5);
  uint16_t base_dmg = get_player_damage(attack_level, S_TIER);
  base_dmg *= 2;
  damage_monster(base_dmg, DAMAGE_MAGICAL);
}

void sorcerer_wild_magic(void) {
  Monster *monster = encounter.monsters;

  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;

    uint8_t roll = d8();

    if (roll == 0) {
      monster->target_hp = 1;
    } else if (roll == 1) {
      monster->target_hp = monster->max_hp - 1;
    } else if (roll < 4) {
      apply_agl_down(
        monster->status_effects, A_TIER, 10, monster->debuff_immune);
      apply_def_down(
        monster->status_effects, A_TIER, 10, monster->debuff_immune);
      apply_atk_down(
        monster->status_effects, A_TIER, 10, monster->debuff_immune);
    } else if (roll < 6) {
      apply_confused(
        monster->status_effects, A_TIER, 10, monster->debuff_immune);
      apply_blind(
        monster->status_effects, A_TIER, 10, monster->debuff_immune);
    } else if (roll == 6) {
      sorcerer_fireball();
    } else {
      sorcerer_sleetstorm();
    }
  }

  sprintf(battle_pre_message, str_player_sorc_wild_magic);
  SKIP_POST_MSG;
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
    PLAYER_MISS_ALL;
  } else {
    sprintf(battle_post_message, "Damage Applied.");
  }
}

void test_class_ability1(void) {
  sprintf(battle_pre_message, "(De)buffing\x60");
  SKIP_POST_MSG;

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
  SKIP_POST_MSG;
  Monster *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;
    monster->target_hp = 0;
  }
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

char *get_druid_grant_message(AbilityFlag flag) {
  switch (flag) {
  case ABILITY_1:
    return str_gain_ability_druid1;
  case ABILITY_2:
    return str_gain_ability_druid2;
  case ABILITY_3:
    return str_gain_ability_druid3;
  case ABILITY_4:
    return str_gain_ability_druid4;
  default:
    return str_gain_ability_druid5;
  }
}

char *get_fighter_grant_message(AbilityFlag flag) {
  switch (flag) {
  case ABILITY_1:
    return str_gain_ability_fighter1;
  case ABILITY_2:
    return str_gain_ability_fighter2;
  case ABILITY_3:
    return str_gain_ability_fighter3;
  case ABILITY_4:
    return str_gain_ability_fighter4;
  default:
    return str_gain_ability_fighter5;
  }
}

char *get_monk_grant_message(AbilityFlag flag) {
  switch (flag) {
  case ABILITY_1:
    return str_gain_ability_monk1;
  case ABILITY_2:
    return str_gain_ability_monk2;
  case ABILITY_3:
    return str_gain_ability_monk3;
  case ABILITY_4:
    return str_gain_ability_monk4;
  default:
    return str_gain_ability_monk5;
  }
}

char *get_sorcerer_grant_message(AbilityFlag flag) {
  switch (flag) {
  case ABILITY_1:
    return str_gain_ability_sorcerer1;
  case ABILITY_2:
    return str_gain_ability_sorcerer2;
  case ABILITY_3:
    return str_gain_ability_sorcerer3;
  case ABILITY_4:
    return str_gain_ability_sorcerer4;
  default:
    return str_gain_ability_sorcerer5;
  }
}

const char *get_grant_message(AbilityFlag flag) BANKED {
  switch (player.player_class) {
  case CLASS_DRUID:
    return get_druid_grant_message(flag);
  case CLASS_FIGHTER:
    return get_fighter_grant_message(flag);
  case CLASS_MONK:
    return get_monk_grant_message(flag);
  default:
    return get_sorcerer_grant_message(flag);
  }
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

  switch (player.player_class) {
  case CLASS_DRUID:
    sprintf(player.name, "Lyra");
    break;
  case CLASS_FIGHTER:
    sprintf(player.name, "Deneth");
    break;
  case CLASS_MONK:
    sprintf(player.name, "Ken");
    break;
  case CLASS_SORCERER:
    sprintf(player.name, "Tyrion");
    break;
  }

  player.has_torch = false;
  player.magic_keys = 0;
  clear_inventory();

  set_class_abilities();
  player.ability_flags = 0;

  grant_ability(ABILITY_0);
  set_player_level(4);
  reset_player_stats();

  player.message_speed = AUTO_PAGE_MED;
  player.aspect_immune = 0;
  player.aspect_resist = 0;
  player.aspect_vuln = 0;
}

bool level_up(uint16_t xp) BANKED {
  bool level_up = false;
  player.exp += xp;

  while (player.exp >= player.next_level_exp) {
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
