#pragma bank 0

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
  i->can_flee = true;
  i->fled = false;
  i->parameter = 0;
}

/**
 * Applies damage to the player.
 * @param base_damage Base damage for the attck.
 * @param type Type of damage dealt.
 */
void damage_player(uint16_t base_damage, DamageAspect type) {
  if (player.aspect_immune & type) {
    sprintf(battle_post_message, str_battle_monster_hit_immune);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = roll >= 12;

  if (critical) {
    sprintf(battle_post_message, str_battle_monster_hit_crit, damage);
  } else  if (player.aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_battle_monster_hit_resist, damage);
  } else if (player.aspect_vuln & type) {
    damage <<= 1;
  } else if (type == DAMAGE_PHYSICAL) {
    sprintf(battle_post_message, str_battle_monster_hit, damage);
  } else {
    const char *aspect = damage_aspect_name(type);
    sprintf(battle_post_message, str_battle_monster_hit_aspect, damage, aspect);
  }

  if (player.hp < damage)
    player.hp = 0;
  else
    player.hp -= damage;
}


// -----------------------------------------------------------------------------
// Monster 255 - Test Dummy
// -----------------------------------------------------------------------------
const Tileset test_dummy_tileset = { MONSTER_TILES, 14, tile_dummy };
const Monster MONSTER_DUMMY = { 255, "Dummy", &test_dummy_tileset };

const palette_color_t DUMMY_COLORS[4] = {
  RGB_WHITE,
  RGB8(210, 0, 150),
  RGB_DARKBLUE,
  RGB8(24, 0, 24),
};

void dummy_take_turn(MonsterInstance *dummy) {
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
    sprintf(battle_pre_message, str_battle_monster_attack,
      dummy->monster->name, dummy->id);
    if (roll_attack(dummy->atk, player.def)) {
      const uint16_t base = get_monster_dmg(dummy->level, dummy->exp_tier);
      damage_player(base, DAMAGE_PHYSICAL);
    } else {
      sprintf(battle_post_message, str_battle_monster_miss);
    }
    break;
  default:
    sprintf(battle_post_message, str_monster_dummy_post);
    break;
  }
}

void dummy_generator(MonsterInstance *m, uint8_t level, TestDummyType type) {
  monster_init_instance(m, &MONSTER_DUMMY);

  PowerTier tier = C_TIER;

  m->palette = DUMMY_COLORS;
  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level, tier);
  m->hp = m->max_hp;

  m->atk_base = get_monster_atk(level, tier);
  m->def_base = get_monster_def(level, tier);
  m->matk_base = get_monster_atk(level, tier);
  m->mdef_base = get_monster_def(level, tier);
  m->agl_base = get_agl(1, C_TIER);

  m->take_turn = dummy_take_turn;
  m->parameter = type;

  monster_reset_stats(m);

}

// -----------------------------------------------------------------------------
// Monster 1 - Kobold
// -----------------------------------------------------------------------------
const Tileset kobold_tileset = { MONSTER_TILES, 14, tile_kobold };
const Monster MONSTER_KOBOLD = { 1, "Kobold", &kobold_tileset };

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
const Tileset beholder_tileset = { MONSTER_TILES, 14, tile_beholder };
const Monster MONSTER_BEHOLDER = { 2, "BEHOLDER", &beholder_tileset };
