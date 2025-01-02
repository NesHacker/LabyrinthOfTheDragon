#pragma bank 6

#include <stdio.h>

#include "battle.h"
#include "data.h"
#include "monster.h"
#include "stats.h"
#include "strings.h"

/**
 * Initializes a monster instance for a monster generator.
 * @param i Monster instance to reset.
 * @param m Base monster for the instance.
 */
static void monster_init_instance(
  Monster *monster,
  MonsterType type,
  const char *name,
  const Tileset *ts
) {
  monster->type = type;
  monster->tileset = ts;
  monster->active = true;
  monster->name = name;
  monster->palette = NULL;
  monster->id = '\0';
  monster->exp_tier = C_TIER;
  monster->hp = 0;
  monster->max_hp = 0;
  monster->target_hp = 0;
  monster->atk_base = 0;
  monster->atk = 0;
  monster->def_base = 0;
  monster->def = 0;
  monster->matk_base = 0;
  monster->matk = 0;
  monster->mdef_base = 0;
  monster->mdef = 0;
  monster->agl_base = 0;
  monster->agl = 0;
  monster->aspect_immune = 0;
  monster->aspect_resist = 0;
  monster->aspect_vuln = 0;
  monster->debuff_immune = 0;
  monster->can_flee = true;
  monster->fled = false;
  monster->parameter = 0;
  monster->trip_turns = 0;
}

/**
 * Applies damage to the player.
 * @param base_damage Base damage for the attck.
 * @param type Type of damage dealt.
 */
static uint16_t damage_player(uint16_t base_damage, DamageAspect type) {
  if (player.aspect_immune & type) {
    sprintf(battle_post_message, str_monster_hit_immune);
    return 0;
  }

  // Monk evasion
  if (player.special_flags & SPECIAL_EVASION) {
    uint8_t evade_chance = 3;
    if (player.level > 70)
      evade_chance = 5;
    else if (player.level > 30)
      evade_chance = 4;
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

  if (player.hp < damage)
    damage = player.hp;
  player.hp -= damage;

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

void monster_take_turn(Monster *monster) BANKED {
  if (!monster->take_turn) {
    sprintf(battle_pre_message, "ERROR:\nNO take_turn()");
    skip_post_message = true;
  }
  monster->take_turn(monster);
}

// -----------------------------------------------------------------------------
// Monster 1 - Kobold
// -----------------------------------------------------------------------------

#define KOBOLD_PRONE_FLAG FLAG(7)

static void kobold_take_turn(Monster *m) {
  uint8_t daze_chance;
  uint8_t prone_chance;

  switch (m->exp_tier) {
  case C_TIER:
    daze_chance = 13;
    prone_chance = 14;
    break;
  case B_TIER:
    daze_chance = 14;
    prone_chance = 15;
    break;
  case A_TIER:
    daze_chance = 15;
    prone_chance = 16;
    break;
  default:
    daze_chance = 16;
    prone_chance = 16;
  }

  if (m->parameter & KOBOLD_PRONE_FLAG) {
    // If they fell prone, they spend a whole turn getting up
    m->parameter &= ~KOBOLD_PRONE_FLAG;
    sprintf(battle_pre_message, str_monster_kobold_get_up, m->id);
    skip_post_message = true;
    return;
  }

  const uint8_t move_roll = d16();

  // Kobolds sometimes space out entirely
  if (move_roll >= daze_chance) {
    sprintf(battle_pre_message, str_monster_kobold_dazed, m->id);
    skip_post_message = true;
    return;
  }

  uint8_t atk, def;
  DamageAspect type;

  if (move_roll < 4) {
    // Fire loogie
    atk = m->matk;
    def = player.mdef;
    type = DAMAGE_FIRE;
    sprintf(battle_pre_message, str_monster_kobold_fire, m->id);
  } else {
    // Axe attack
    atk = m->atk;
    def = player.def;
    type = DAMAGE_PHYSICAL;
    sprintf(battle_pre_message, str_monster_kobold_axe, m->id);
  }

  if (roll_attack(atk, def)) {
    uint16_t base_damage = get_monster_dmg(m->level, m->exp_tier);
    clear_debug();
    damage_player(base_damage, type);
  } else if (d16() >= prone_chance) {
    // Not only did they miss, but they fell prone and need to spend a turn
    // getting back up...
    sprintf(battle_post_message, str_monster_kobold_miss);
    m->parameter |= KOBOLD_PRONE_FLAG;
  } else {
    sprintf(battle_post_message, str_monster_miss);
  }
}

void kobold_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_KOBOLD, str_misc_kobold, &kobold_tileset);

  m->palette = kobold_palettes + tier * 4;
  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level_offset(level, -2), tier);
  m->hp = m->max_hp;

  m->atk_base = get_monster_atk(level_offset(level, -3), tier);
  m->def_base = get_monster_def(level_offset(level, -2), tier);
  m->matk_base = get_monster_atk(level_offset(level, -1), tier);
  m->mdef_base = get_monster_def(level_offset(level, -4), tier);
  m->agl_base = get_agl(level_offset(level, 2), tier == C_TIER ? B_TIER : tier);

  m->aspect_resist = DAMAGE_EARTH;
  m->aspect_vuln = DAMAGE_FIRE;

  m->take_turn = kobold_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 2 - Goblin
// -----------------------------------------------------------------------------

static void goblin_take_turn(Monster *monster) {
  uint8_t nose_pick_chance = 14;
  uint8_t flee_chance = 12;

  if (monster->exp_tier > C_TIER) {
    nose_pick_chance = 15;
    flee_chance = 14;
  }

  if (monster->exp_tier > B_TIER)
    nose_pick_chance = 16;

  // See if the goblin forgets it's fighting and picks its nose
  if (d16() >= nose_pick_chance) {
    sprintf(battle_pre_message, str_monster_goblin_nose_pick, monster->id);
    skip_post_message = true;
    return;
  }

  // Goblins sometimes flee at B_TIER and lower
  if (monster->exp_tier <= B_TIER) {
    uint16_t hp_pct = monster->hp * 16;
    hp_pct /= monster->max_hp;
    if (hp_pct <= 4 && d16() >= flee_chance) {
      monster_flee(monster);
      return;
    }
  }

  uint8_t atk, def;
  DamageAspect type;
  PowerTier damage_tier;

  if (d16() >= 12) {
    // Acid Arrow
    sprintf(battle_pre_message, str_monster_goblin_acid_arrow, monster->id);
    atk = monster->atk;
    def = player.mdef;
    type = DAMAGE_MAGICAL;
    damage_tier = (monster->exp_tier == C_TIER) ? B_TIER : monster->exp_tier;
  } else {
    // Shortsword
    sprintf(battle_pre_message, str_monster_goblin_attack, monster->id);
    atk = monster->atk;
    def = player.def;
    type = DAMAGE_PHYSICAL;
    damage_tier = monster->exp_tier;
  }

  if (roll_attack(atk, def))
    damage_player(get_monster_dmg(monster->level - 1, damage_tier), type);
  else
    sprintf(battle_post_message, str_monster_miss);
}

void goblin_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_GOBLIN, str_misc_goblin, &goblin_tileset);
  m->palette = goblin_palettes;
  m->take_turn = goblin_take_turn;

  m->exp_tier = tier;
  m->level = level + 1;

  m->max_hp = get_monster_hp(level_offset(level, 0), tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level_offset(level, -2), tier);
  m->matk_base = get_monster_atk(level_offset(level, -1), tier);
  m->mdef_base = get_monster_def(level_offset(level, -3), tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 3 - Zombie
// -----------------------------------------------------------------------------

#define ZOMBIE_BITE_HIT_FLAG FLAG(7)

static void zombie_take_turn(Monster *monster) {
  uint8_t bite_chance = 15;
  if (monster->exp_tier == B_TIER)
    bite_chance = 14;
  if (monster->exp_tier == A_TIER)
    bite_chance = 12;
  if (monster->exp_tier == S_TIER)
    bite_chance = 8;

  // Zombies will only try to bite if they haven't hit with it yet
  if (
    !(monster->parameter & ZOMBIE_BITE_HIT_FLAG) &&
    d16() >= bite_chance
  ) {
    sprintf(battle_pre_message, str_monster_zombie_brains);
    if (roll_attack(monster->atk, player.def)) {
      monster->parameter |= ZOMBIE_BITE_HIT_FLAG;
      uint8_t duration = 2;
      PowerTier poison_tier = C_TIER;
      if (monster->exp_tier > C_TIER) {
        duration = 4;
        poison_tier = B_TIER;
      }
      apply_poison(
        encounter.player_status_effects,
        poison_tier,
        duration,
        player.debuff_immune);
      sprintf(battle_post_message, str_monster_zombie_bite_hit, monster->id);
    } else {
      sprintf(battle_post_message, str_monster_zombie_bite_miss, monster->id);
    }
    return;
  }

  sprintf(battle_pre_message, str_monster_zombie_slam, monster->id);

  if (roll_attack(monster->atk, player.def)) {
    damage_player(
      get_monster_dmg(monster->level, monster->exp_tier), DAMAGE_PHYSICAL);
  }
  else
    sprintf(battle_post_message, str_monster_miss);
}

void zombie_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_ZOMBIE, str_misc_zombie, &zombie_tileset);
  m->palette = zombie_palettes;
  m->take_turn = zombie_take_turn;

  m->exp_tier = tier;
  m->level = level + 2;

  m->max_hp = get_monster_hp(level_offset(level, 2), tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level_offset(level, -2), tier);
  m->def_base = get_monster_def(level_offset(level, -3), tier);
  m->matk_base = get_monster_atk(level_offset(level, -2), tier);
  m->mdef_base = get_monster_def(level_offset(level, -3), tier);
  m->agl_base = get_agl(level_offset(level, -5), tier);

  m->aspect_resist = DAMAGE_DARK;
  m->aspect_vuln = DAMAGE_FIRE | DAMAGE_MAGICAL;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 4 - Bugbear
// -----------------------------------------------------------------------------

static void bugbear_take_turn(Monster *monster) {
  if (
    monster->parameter > 0 &&
    d8() < 2
  ) {
    sprintf(battle_pre_message, str_monster_bugbear_for_hruggek, monster->id);
    if (roll_attack(monster->atk, player.mdef)) {
      apply_scared(encounter.player_status_effects, C_TIER, 2, 0);
      sprintf(battle_post_message, str_monster_bugbear_for_hruggek_hit);
      monster->parameter--;
    } else {
      sprintf(battle_post_message, str_monster_bugbear_for_hruggek_miss);
    }
    return;
  }

  uint8_t atk;
  uint16_t base_damage;
  PowerTier tier;

  switch (monster->exp_tier) {
  case S_TIER:
    tier = S_TIER;
    break;
  case A_TIER:
  case B_TIER:
    tier = B_TIER;
    break;
  default:
    tier = C_TIER;
  }

  if (d8() < 3) {
    atk = get_monster_atk(level_offset(monster->level, 4), monster->exp_tier);
    base_damage = get_monster_dmg(level_offset(monster->level, -2), tier);
    sprintf(battle_pre_message, str_monster_bugbear_javelin, monster->id);
  } else {
    atk = monster->atk;
    base_damage = get_monster_dmg(level_offset(monster->level, 2), tier);
    sprintf(battle_pre_message, str_monster_bugbead_club, monster->id);
  }

  if (roll_attack(atk, player.def))
    damage_player(base_damage, DAMAGE_PHYSICAL);
  else
    sprintf(battle_post_message, str_monster_miss);

}

void bugbear_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_BUGBEAR, str_misc_bugbear, &bugbear_tileset);
  m->palette = bugbear_palettes;
  m->take_turn = bugbear_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level_offset(level, 1), tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level_offset(level, 2), tier);
  m->agl_base = get_agl(level_offset(level, 3), tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = DEBUFF_BLIND | DEBUFF_CONFUSED | DEBUFF_POISONED;

  switch (tier) {
  case C_TIER:
    m->parameter = 1;
    break;
  case A_TIER:
  case B_TIER:
    m->parameter = 2;
    break;
  default:
    m->parameter = 4;
  }

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 5 - Owlbear
// -----------------------------------------------------------------------------

static void owlbear_take_turn(Monster *monster) {
  PowerTier tier = C_TIER;
  if (monster->exp_tier == S_TIER)
    tier = A_TIER;
  else if (monster->exp_tier > C_TIER)
    tier = B_TIER;

  if (monster->parameter > 0 && d8() < 4) {
    // Pounce
    sprintf(battle_pre_message, str_monster_owlbear_pounce, monster->id);
    if (roll_attack(monster->atk, player.def)) {
      uint16_t base_damage = get_monster_dmg(monster->level, tier);
      if (d8() < 2) {
        // Toppled
        monster->parameter--;
        uint16_t dmg = damage_player(base_damage, DAMAGE_PHYSICAL);
        player.trip_turns = 1;
        sprintf(battle_post_message, str_monster_owlbear_pounce_topple, dmg);
      } else {
        damage_player(base_damage, DAMAGE_PHYSICAL);
      }
    } else {
      sprintf(battle_post_message, str_monster_owlbear_pounce_miss);
    }
    return;
  }
  if (d8() < 2) {
    // Multi-attack
    sprintf(battle_pre_message, str_monster_owlbear_multi, monster->id);

    uint16_t damage = 0;
    if (roll_attack(monster->atk, player.def))
      damage += get_monster_dmg(level_offset(monster->level, -2), tier);
    if (roll_attack(monster->atk, player.def))
      damage += get_monster_dmg(monster->level, tier);

    if (damage > 0)
      damage_player(damage, DAMAGE_PHYSICAL);
    else
      sprintf(battle_post_message, str_monster_miss);
  } else {
    // Beak only
    sprintf(battle_pre_message, str_monster_owlbear_beak, monster->id);
    if (roll_attack(monster->atk, player.def)) {
      damage_player(get_monster_dmg(
        level_offset(monster->level, -2), tier),
        DAMAGE_PHYSICAL);
    } else {
      sprintf(battle_post_message, str_monster_miss);
    }
  }
}

void owlbear_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_OWLBEAR, str_misc_owlbear, &owlbear_tileset);
  m->palette = owlbear_palettes;
  m->take_turn = owlbear_take_turn;

  m->exp_tier = tier;
  m->level = level + 2;

  m->max_hp = get_monster_hp(level_offset(level, 2), tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level_offset(level, 2), tier);
  m->def_base = get_monster_def(level_offset(level, -2), tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level_offset(level, -5), tier);
  m->agl_base = get_agl(level_offset(level, 3), tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  switch (tier) {
  case C_TIER:
    m->parameter = 1;
    break;
  case B_TIER:
    m->parameter = 2;
    break;
  case A_TIER:
    m->parameter = 3;
    break;
  default:
    m->parameter = 5;
  }

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 6 - Gelatinous Cube
// -----------------------------------------------------------------------------

static void gelatinous_cube_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void gelatinous_cube_generator(
  Monster *m,
  uint8_t level,
  PowerTier tier
) BANKED {
  monster_init_instance(
    m, MONSTER_GELATINOUS_CUBE,
    str_misc_gelatinous_cube, &gelatinous_cube_tileset);
  m->palette = gelatinous_cube_palettes;
  m->take_turn = goblin_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 7 - Displacer Beast
// -----------------------------------------------------------------------------

static void displacer_beast_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void displacer_beast_generator(
  Monster *m,
  uint8_t level,
  PowerTier tier
) BANKED {
  monster_init_instance(
    m, MONSTER_DISPLACER_BEAST,
    str_misc_displacer_beast, &displacer_beast_tileset);
  m->palette = displacer_beast_palettes;
  m->take_turn = displacer_beast_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 8 - Will-o-Wisp
// -----------------------------------------------------------------------------

static void will_o_wisp_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void will_o_wisp_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_WILL_O_WISP, str_misc_will_o_wisp, &will_o_wisp_tileset);
  m->palette = will_o_wisp_palettes;
  m->take_turn = will_o_wisp_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 9 - Death Knight
// -----------------------------------------------------------------------------

static void deathknight_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void deathknight_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_DEATHKNIGHT, str_misc_death_knight, &deathknight_tileset);
  m->palette = deathknight_palettes;
  m->take_turn = deathknight_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 10 - Mind Flayer
// -----------------------------------------------------------------------------

static void mindflayer_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void mindflayer_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_MINDFLAYER, str_misc_mind_flayer, &mindflayer_tileset);
  m->palette = mindflayer_palettes;
  m->take_turn = mindflayer_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 11 - Beholder
// -----------------------------------------------------------------------------

static void beholder_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void beholder_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_BEHOLDER, str_misc_beholder, &beholder_tileset);
  m->palette = beholder_palettes;
  m->take_turn = beholder_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 12 - Dragon
// -----------------------------------------------------------------------------

static void dragon_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void dragon_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_DRAGON, str_misc_dragon, &dragon_tileset);
  m->palette = dragon_palettes;
  m->take_turn = dragon_take_turn;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(level, tier);

  m->aspect_resist = 0;
  m->aspect_vuln = 0;
  m->debuff_immune = 0;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 255 - Test Dummy
// -----------------------------------------------------------------------------

static void dummy_take_turn(Monster *dummy) {
  sprintf(battle_pre_message, str_monster_dummy_pre, dummy->id);

  switch (dummy->parameter) {
  case DUMMY_INVINCIBLE:
    dummy->target_hp = dummy->max_hp;
    sprintf(battle_post_message, str_monster_dummy_post_heal);
    break;
  case DUMMY_COWARD:
    monster_flee(dummy);
    break;
  case DUMMY_AGGRESSIVE:
    sprintf(battle_pre_message, str_monster_attack,
      dummy->name, dummy->id);
    if (roll_attack(dummy->atk, player.def)) {
      const uint16_t base = get_monster_dmg(dummy->level, dummy->exp_tier);
      damage_player(base, DAMAGE_PHYSICAL);
    } else {
      sprintf(battle_post_message, str_monster_miss);
    }
    break;
  default:
    skip_post_message = true;
    break;
  }
}

void dummy_generator(Monster *m, uint8_t level, TestDummyType type) BANKED {
  monster_init_instance(
    m,
    MONSTER_DUMMY,
    str_misc_dummy,
    &test_dummy_tileset
  );

  PowerTier tier = C_TIER;

  m->palette = dummy_palette;
  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;

  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(1, C_TIER);

  m->parameter = type;
  m->take_turn = dummy_take_turn;

  monster_reset_stats(m);
}
