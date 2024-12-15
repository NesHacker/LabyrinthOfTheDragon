#pragma bank 3

#include <stdio.h>

#include "battle.h"
#include "data.h"
#include "monster.h"
#include "stats.h"
#include "strings.h"

void monster_init_instance(MonsterInstance *i, Monster *m) {
  i->active = true;
  i->monster = m;
  i->palette = NULL;
  i->id = '\0';
  i->exp_tier = C_TIER;
  i->hp = 0;
  i->max_hp = 0;
  i->target_hp = 0;
  i->atk_base = 0;
  i->atk = 0;
  i->def_base = 0;
  i->def = 0;
  i->matk_base = 0;
  i->matk = 0;
  i->mdef_base = 0;
  i->mdef = 0;
  i->agl_base = 0;
  i->agl = 0;
  i->aspect_immune = 0;
  i->aspect_resist = 0;
  i->aspect_vuln = 0;
  i->debuff_immune = 0;
  i->parameter = 0;
}

void monster_reset_stats(MonsterInstance *m) {
  m->agl = m->agl_base;
  m->atk = m->atk_base;
  m->def = m->def_base;
  m->matk = m->matk_base;
  m->mdef = m->mdef_base;
  m->target_hp = m->hp;
}

/**
 * Applies damage to the player.
 * @param base_damage Base damage for the attck.
 * @param type Type of damage dealt.
 */
void damage_player(uint16_t base_damage, DamageAspect type) {
  Summon *summon = player.summon;

  if (summon->aspect_immune & type) {
    sprintf(battle_post_message, str_battle_monster_hit_immune);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = roll >= 12;

  if (critical) {
    sprintf(battle_post_message, str_battle_monster_hit_crit, damage);
  } else  if (summon->aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_battle_monster_hit_resist, damage);
  } else if (summon->aspect_vuln & type) {
    damage <<= 1;
  } else if (type == DAMAGE_PHYSICAL) {
    sprintf(battle_post_message, str_battle_monster_hit, damage);
  } else {
    const char *aspect = damage_aspect_name(type);
    sprintf(battle_post_message, str_battle_monster_hit_aspect, damage, aspect);
  }

  if (player.target_hp < damage)
    player.target_hp = 0;
  else
    player.target_hp -= damage;
}


// -----------------------------------------------------------------------------
// Monster 1 - Kobold
// -----------------------------------------------------------------------------
const Monster MONSTER_KOBOLD = {
  // id, name
  1, "KOBOLD",
  // tile_bank, tile_data, palette
  4, tile_kobold,
};

const palette_color_t MONSTER_KOBOLD_PALETTES[16] = {
  // C-Tier
  RGB_WHITE,
  RGB8(177, 113, 51),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // B-Tier
  RGB_WHITE,
  RGB8(113, 51, 177),
  RGB8(77, 22, 11),
  RGB8(37, 3, 40),
  // A-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
  // S-Tier
  RGB_WHITE,
  RGB_LIGHTGRAY,
  RGB_DARKGRAY,
  RGB_BLACK,
};


void kobold_take_turn(MonsterInstance *m) {
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
    sprintf(battle_post_message, str_battle_monster_miss);
  }
}

void kobold_generator(MonsterInstance *m, uint8_t level, PowerTier tier) {
  monster_init_instance(m, &MONSTER_KOBOLD);

  m->palette = MONSTER_KOBOLD_PALETTES + tier * 4;
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
// Monster 2 - Beholder
// -----------------------------------------------------------------------------
const palette_color_t MONSTER_BEHOLDER_PALETTES[4] = {
  // C-Tier
  RGB_WHITE,
  RGB8(209, 206, 107),
  RGB8(126, 73, 73),
  RGB8(0, 40, 51),
};

const Monster MONSTER_BEHOLDER = {
  // id, name
  2, "BEHOLDER",
  // tile_bank, tile_data, palette
  4, tile_beholder,
};
