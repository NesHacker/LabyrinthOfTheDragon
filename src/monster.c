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
void monster_init_instance(
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
}

/**
 * Applies damage to the player.
 * @param base_damage Base damage for the attck.
 * @param type Type of damage dealt.
 */
void damage_player(uint16_t base_damage, DamageAspect type) {
  if (player.aspect_immune & type) {
    sprintf(battle_post_message, str_monster_hit_immune);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = roll >= 12;

  if (critical) {
    sprintf(battle_post_message, str_monster_hit_crit, damage);
  } else  if (player.aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_monster_hit_resist, damage);
  } else if (player.aspect_vuln & type) {
    damage <<= 1;
  } else if (type == DAMAGE_PHYSICAL) {
    sprintf(battle_post_message, str_monster_hit, damage);
  } else {
    const char *aspect = damage_aspect_name(type);
    sprintf(battle_post_message, str_monster_hit_aspect, damage, aspect);
  }

  if (player.hp < damage)
    player.hp = 0;
  else
    player.hp -= damage;
}

// -----------------------------------------------------------------------------
// Monster 255 - Test Dummy
// -----------------------------------------------------------------------------

void dummy_take_turn(Monster *dummy) {
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

// -----------------------------------------------------------------------------
// Monster 1 - Kobold
// -----------------------------------------------------------------------------

void kobold_take_turn(Monster *m) {
  const uint8_t move_roll = d16();

  // Dazed (silly kobolds being dazed 6.25% of the time)
  if (move_roll == 15) {
    sprintf(battle_pre_message, str_monster_kobold_dazed, m->id);
    sprintf(battle_post_message, str_monster_kobold_does_nothing);
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
    damage_player(get_monster_dmg(m->level, m->exp_tier), type);
  } else if (!d16()) {
    // Silly kobolds falling over on their ass 6.25% of the time
    sprintf(battle_post_message, str_monster_kobold_miss);
  } else {
    sprintf(battle_post_message, str_monster_miss);
  }
}

void kobold_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_KOBOLD, str_misc_kobold, &kobold_tileset);

  m->palette = kobold_palettes + tier * 4;
  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level_offset(level, 2), tier);
  m->hp = m->max_hp;

  m->atk_base = get_monster_atk(level_offset(level, 3), tier);
  m->def_base = get_monster_def(level_offset(level, -1), tier);
  m->matk_base = get_monster_atk(level_offset(level, -1), tier);
  m->mdef_base = get_monster_def(level_offset(level, -2), tier);
  m->agl_base = get_agl(level_offset(level, 2), tier == C_TIER ? B_TIER : tier);

  m->aspect_resist = DAMAGE_EARTH;
  m->aspect_vuln = DAMAGE_FIRE;

  m->take_turn = kobold_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 2 - Goblin
// -----------------------------------------------------------------------------

void goblin_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void goblin_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_GOBLIN, str_misc_goblin, &goblin_tileset);
  m->palette = goblin_palettes;
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
// Monster 3 - Zombie
// -----------------------------------------------------------------------------

void zombie_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void zombie_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_ZOMBIE, str_misc_zombie, &zombie_tileset);
  m->palette = zombie_palettes;
  m->take_turn = zombie_take_turn;

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
// Monster 4 - Bugbear
// -----------------------------------------------------------------------------

void bugbear_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void bugbear_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_BUGBEAR, str_misc_bugbear, &bugbear_tileset);
  m->palette = bugbear_palettes;
  m->take_turn = bugbear_take_turn;

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
// Monster 5 - Owlbear
// -----------------------------------------------------------------------------

void owlbear_take_turn(Monster *monster) {
  sprintf(battle_pre_message, str_monster_does_nothing,
    monster->name, monster->id);
  skip_post_message = true;
}

void owlbear_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(m, MONSTER_OWLBEAR, str_misc_owlbear, &owlbear_tileset);
  m->palette = owlbear_palettes;
  m->take_turn = owlbear_take_turn;

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
// Monster 6 - Gelatinous Cube
// -----------------------------------------------------------------------------

void gelatinous_cube_take_turn(Monster *monster) {
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

void displacer_beast_take_turn(Monster *monster) {
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

void will_o_wisp_take_turn(Monster *monster) {
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

void deathknight_take_turn(Monster *monster) {
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

void mindflayer_take_turn(Monster *monster) {
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

void beholder_take_turn(Monster *monster) {
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

void dragon_take_turn(Monster *monster) {
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
// Common monster routines.
// -----------------------------------------------------------------------------

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
