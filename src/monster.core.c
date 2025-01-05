#pragma bank 6

#include <stdio.h>

#include "battle.h"
#include "data.h"
#include "monster.h"
#include "stats.h"
#include "strings.h"

/**
 * @return The string name for the given aspect type.
 * @param type Aspect type to convert.
 */
const char *damage_aspect_name(DamageAspect type) BANKED {
  switch (type) {
  case DAMAGE_PHYSICAL:
    return str_misc_physical;
  case DAMAGE_MAGICAL:
    return str_misc_magical;
  case DAMAGE_EARTH:
    return str_misc_earth;
  case DAMAGE_WATER:
    return str_misc_water;
  case DAMAGE_AIR:
    return str_misc_air;
  case DAMAGE_FIRE:
    return str_misc_fire;
  case DAMAGE_LIGHT:
    return str_misc_light;
  default:
    return str_misc_dark;
  }
}

void monster_init_instance(
  Monster *monster,
  MonsterType type,
  const char *name,
  const Tileset *ts,
  uint8_t level,
  PowerTier tier
) BANKED {
  monster->type = type;
  monster->tileset = ts;
  monster->active = true;
  monster->name = name;
  monster->palette = NULL;
  monster->id = '\0';
  monster->level = level;
  monster->exp_level = level;
  monster->exp_tier = tier;
  monster->hp = get_monster_hp(level, tier);
  monster->max_hp = monster->hp;
  monster->atk_base = get_monster_atk(level, tier);
  monster->def_base = get_monster_def(level, tier);
  monster->matk_base = get_monster_atk(level, tier);
  monster->mdef_base = get_monster_def(level, tier);
  monster->agl_base = get_agl(level, tier);
  monster->aspect_immune = 0;
  monster->aspect_resist = 0;
  monster->aspect_vuln = 0;
  monster->debuff_immune = 0;
  monster->can_flee = true;
  monster->fled = false;
  monster->parameter = 0;
  monster->trip_turns = 0;
}

uint16_t damage_player(uint16_t base_damage, DamageAspect type) BANKED {
  if (player.aspect_immune & type) {
    sprintf(battle_post_message, str_monster_hit_immune);
    return 0;
  }

  if (player.special_flags & SPECIAL_EVASION) {
    uint8_t evade_chance = 4;
    if (player.level > 70)
      evade_chance = 6;
    else if (player.level > 30)
      evade_chance = 5;
    if (d8() < evade_chance) {
      sprintf(battle_post_message, str_battle_monster_miss_evaded);
      return 0;
    }
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = is_critical(roll);
  bool barskin = has_special(SPECIAL_BARKSKIN) && type != DAMAGE_FIRE;

  if (barskin)
    damage >>= 1;

  if (player.aspect_resist & type)
    damage >>= 1;
  else if (player.aspect_vuln & type)
    damage <<= 1;

  if (barskin)
    sprintf(battle_post_message, str_monster_hit_barkskin, damage);
  else if (critical)
    sprintf(battle_post_message, str_monster_hit_crit, damage);
  else if (player.aspect_resist & type)
    sprintf(battle_post_message, str_monster_hit_resist, damage);
  else if (player.aspect_vuln & type)
    sprintf(battle_post_message, str_monster_hit_vuln, damage);
  else if (type == DAMAGE_PHYSICAL)
    sprintf(battle_post_message, str_monster_hit, damage);
  else {
    const char *aspect = damage_aspect_name(type);
    sprintf(battle_post_message, str_monster_hit_aspect, damage, aspect);
  }

  if (damage > player.hp) {
    damage = player.hp;
    player.hp = 0;
  } else {
    player.hp -= damage;
  }

  return damage;
}

void monster_flee(Monster *monster) BANKED {
  sprintf(
    battle_pre_message,
    str_monster_flee,
    monster->name,
    monster->id
  );

  if (roll_flee(monster->agl, player.agl)) {
    sprintf(battle_post_message, str_monster_flee_success);
    monster->fled = true;
  } else {
    sprintf(battle_post_message, str_monster_flee_failure);
    monster->fled = false;
  }
}

void monster_take_turn(Monster *monster) NONBANKED {
  uint8_t _prev_bank = CURRENT_BANK;
  SWITCH_ROM(monster->bank);
  monster->take_turn(monster);
  SWITCH_ROM(_prev_bank);
}
