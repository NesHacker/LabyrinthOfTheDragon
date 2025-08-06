#pragma bank 7

#include <stdio.h>

#include "battle.h"
#include "battle.effects.h"
#include "data.h"
#include "monster.h"
#include "stats.h"
#include "strings.h"

// -----------------------------------------------------------------------------
// Monster 10 - Mind Flayer
// -----------------------------------------------------------------------------

#define MIND_FLAYER_EXTRACT_BRAIN FLAG(6)
#define MIND_FLAYER_MIND_BLAST FLAG(7)

static void mindflayer_take_turn(Monster *monster) {
  const uint8_t tier = monster->exp_tier;
  const uint16_t base_damage = get_monster_dmg(monster->level, tier);

  if (
    d8() < 3 && !(monster->parameter & MIND_FLAYER_MIND_BLAST)
  ) {
    // Mind blast
    sprintf(battle_pre_message,
      str_monster2_mindflayer_mind_blast, monster->id);

    if (player.debuff_immune & DEBUFF_CONFUSED) {
      monster->parameter |= MIND_FLAYER_MIND_BLAST;
      sprintf(battle_post_message, str_monster2_mindflayer_mind_blast_miss);
      SFX_MISS;
    } else if (roll_attack_monster(monster->matk, player.mdef)) {
      monster->parameter |= MIND_FLAYER_MIND_BLAST;
      uint16_t damage = damage_player(base_damage / 2, DAMAGE_MAGICAL);
      sprintf(
        battle_post_message, str_monster2_mindflayer_mind_blast_hit, damage);
      apply_confused(
        encounter.player_status_effects, tier, 3, player.debuff_immune);
    } else {
      sprintf(battle_post_message, str_monster2_monster_miss);
      SFX_FAIL;
    }

    return;
  }

  if (
    !(monster->parameter & MIND_FLAYER_EXTRACT_BRAIN) &&
    (player.debuffs & FLAG_DEBUFF_CONFUSED)
  ) {
    // Extract brain
    monster->parameter |= MIND_FLAYER_EXTRACT_BRAIN;
    sprintf(
      battle_pre_message, str_monster2_mindflayer_extract_brain, monster->id);

    if (roll_attack_monster(level_offset(monster->level, -15), player.def)) {
      sprintf(battle_post_message, str_monster2_mindflayer_extract_brain_hit);
      player.hp = 0;
      SFX_SPECIAL_CRIT;
    } else {
      sprintf(battle_post_message, str_monster2_monster_miss);
      SFX_FAIL;
    }

    return;
  }

  // Tentacle attack
  sprintf(battle_pre_message, str_monster2_mindflayer_tentacle, monster->id);
  if (roll_attack_monster(monster->atk, player.def)) {
    damage_player(base_damage, DAMAGE_PHYSICAL);
  } else {
    sprintf(battle_post_message, str_monster2_monster_miss);
    SFX_MISS;
  }
}

void mindflayer_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_MINDFLAYER, str_misc_mind_flayer, &mindflayer_tileset,
    level, tier);

  m->max_hp = get_monster_hp(level_offset(level, 10), S_TIER);
  m->atk = get_monster_atk(level_offset(level, 5), tier);
  m->mdef = get_monster_def(level_offset(level, 5), tier);
  m->aspect_resist |= DAMAGE_MAGICAL;

  m->palette = mindflayer_palettes + tier * 4;
  m->bank = BANK_7;
  m->take_turn = mindflayer_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 11 - Beholder
// -----------------------------------------------------------------------------

#define BEHOLDER_PARALYZE 0
#define BEHOLDER_FEAR 1
#define BEHOLDER_SLOW 2
#define BEHOLDER_NECRO 3
#define BEHOLDER_ICE 4
#define BEHOLDER_TRIP 5
#define BEHOLDER_FIRE 6
#define BEHOLDER_DEATH 7

static void beholder_take_turn(Monster *monster) {
  const uint8_t ray_chance[4] = { 2, 3, 4, 5 };
  const uint8_t debuff_turns[4] = { 1, 2, 3, 4 };
  const PowerTier debuff_tiers[4] = { C_TIER, B_TIER, B_TIER, A_TIER };
  const PowerTier exp_tier = monster->exp_tier;

  if (d16() < ray_chance[monster->exp_tier] && monster->parameter > 0) {
    // Random Eyestalk Ray
    const bool hit = roll_attack_monster(monster->matk, player.mdef);
    const uint8_t turns = debuff_turns[exp_tier];
    const PowerTier debuff_tier = debuff_tiers[exp_tier];

    sprintf(battle_pre_message, str_monster2_beholder_shoot_ray, monster->id);
    if (!hit) {
      sprintf(battle_post_message, str_monster2_beholder_ray_miss);
      SFX_FAIL;
      return;
    }

    monster->parameter--;

    const uint8_t ray_type = d8();

    const PowerTier ray_tiers[4] = { B_TIER, A_TIER, A_TIER, S_TIER };
    const uint16_t base_damage = get_monster_dmg(
      monster->level, ray_tiers[exp_tier]);

    switch (ray_type) {
    case BEHOLDER_ICE:
      damage_player(base_damage, DAMAGE_WATER);
      return;
    case BEHOLDER_FIRE:
      damage_player(base_damage, DAMAGE_FIRE);
      return;
    }

    uint16_t damage = damage_player(base_damage, DAMAGE_MAGICAL);
    StatusEffectResult debuff_result = STATUS_RESULT_FAILED;

    switch (ray_type) {
    case BEHOLDER_PARALYZE:
      debuff_result = apply_paralyzed(
        encounter.player_status_effects,
        debuff_tier,
        turns,
        player.debuff_immune
      );
      if (debuff_result == STATUS_RESULT_SUCCESS) {
        sprintf(battle_post_message, str_monster2_beholder_ray_paralyze, damage);
        SFX_MAGIC;
      } else {
        sprintf(battle_post_message, str_monster2_beholder_ray_resist);
        SFX_FAIL;
      }
      return;
    case BEHOLDER_FEAR:
      debuff_result = apply_scared(
        encounter.player_status_effects,
        debuff_tier,
        turns,
        player.debuff_immune
      );
      if (debuff_result == STATUS_RESULT_SUCCESS) {
        sprintf(battle_post_message, str_monster2_beholder_ray_fear, damage);
        SFX_MAGIC;
      } else {
        sprintf(battle_post_message, str_monster2_beholder_ray_resist);
        SFX_FAIL;
      }
      return;
    case BEHOLDER_SLOW:
      debuff_result = apply_agl_down(
        encounter.player_status_effects,
        debuff_tier,
        turns,
        player.debuff_immune
      );
      if (debuff_result == STATUS_RESULT_SUCCESS) {
        sprintf(battle_post_message, str_monster2_beholder_ray_slow, damage);
        SFX_MAGIC;
      } else {
        sprintf(battle_post_message, str_monster2_beholder_ray_resist);
        SFX_FAIL;
      }
      return;
    case BEHOLDER_NECRO:
      debuff_result = apply_poison(
        encounter.player_status_effects,
        debuff_tier,
        turns,
        player.debuff_immune
      );
      if (debuff_result == STATUS_RESULT_SUCCESS) {
        sprintf(battle_post_message, str_monster2_beholder_ray_necro, damage);
        SFX_MAGIC;
      } else {
        sprintf(battle_post_message, str_monster2_beholder_ray_resist);
        SFX_FAIL;
      }
      return;
    case BEHOLDER_TRIP:
      player.trip_turns = turns;
      sprintf(battle_post_message, str_monster2_beholder_ray_trip, damage);
      SFX_MELEE;
      return;
    default:
      bool player_died = d256() < 3;
      if (player_died) {
        player.hp = 0;
        sprintf(battle_post_message, str_monster2_beholder_ray_death);
        SFX_SPECIAL_CRIT;
      } else {
        sprintf(battle_post_message, str_monster2_beholder_ray_resist);
        SFX_FAIL;
      }
      return;
    }
  }

  // Bite
  sprintf(battle_pre_message, str_monster2_beholder_bite, monster->id);
  if (roll_attack_monster(monster->atk, player.def)) {
    damage_player(get_monster_dmg(monster->level, exp_tier), DAMAGE_PHYSICAL);
  } else {
    sprintf(battle_post_message, str_monster2_beholder_bite_miss);
    SFX_MISS;
  }
}

void beholder_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_BEHOLDER, str_misc_beholder, &beholder_tileset, level, tier);

  m->palette = beholder_palettes + tier * 4;
  m->bank = BANK_7;
  m->take_turn = beholder_take_turn;

  m->exp_level = level_offset(level, 10);

  m->max_hp = get_monster_hp(level_offset(level, 10), tier);
  m->atk = get_monster_atk(level_offset(level, 5), tier);
  m->matk = get_monster_atk(level_offset(level, 7), tier);
  m->agl = get_agl(level_offset(level, -4), tier);

  const uint8_t eye_ray_tries[4] = { 1, 2, 3, 4 };
  m->parameter = eye_ray_tries[m->exp_tier];

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 12 - Dragon
// -----------------------------------------------------------------------------

#define DRAGON_FIREBREATH_MASK 0b10000000
#define DRAGON_FRIGHT_MASK     0b00001100
#define DRAGON_LEGEND_MASK     0b00000011

inline uint8_t init_dragon_parameter(
  Monster *m,
  bool firebreath,
  uint8_t fright_actions,
  uint8_t legendary_actions
) {
  m->parameter = (firebreath << 7) | (fright_actions << 2) | legendary_actions;
}

inline uint8_t dragon_legendary(Monster *m) {
  return (m->parameter & DRAGON_LEGEND_MASK);
}

inline void dragon_use_legendary(Monster *m) {
  if (dragon_legendary(m) == 0)
    return;
  m->parameter--;
}

inline bool dragon_firebreath(Monster *m) {
  return (m->parameter & DRAGON_FIREBREATH_MASK);
}

inline void dragon_use_firebreath(Monster *m) {
  m->parameter &= ~DRAGON_FIREBREATH_MASK;
}

inline void dragon_restore_firebreath(Monster *m) {
  m->parameter |= DRAGON_FIREBREATH_MASK;
}

inline uint8_t dragon_fright(Monster *m) {
  return (m->parameter & DRAGON_FRIGHT_MASK) >> 2;
}

inline void dragon_use_fright(Monster *m) {
  uint8_t fright = dragon_fright(m);
  if (fright == 0)
    return;
  fright--;
  m->parameter = (m->parameter & ~DRAGON_FRIGHT_MASK) | (fright << 2);
}

#define LEGENDARY_TAIL_WHIP 0
#define LEGENDARY_WING_FLAP 1

static void dragon_take_turn(Monster *monster) {
  const uint8_t move_roll = d4();

  if (dragon_legendary(monster) && move_roll < 2) {
    // Use legendary action
    dragon_use_legendary(monster);
    const bool hit = roll_attack_monster(monster->atk + 5, player.def);
    const uint8_t legendary_move = d2();

    switch (legendary_move) {
    case LEGENDARY_TAIL_WHIP:
      sprintf(
        battle_pre_message, str_monster2_dragon_legendary_tail, monster->id);
      if (hit) {
        damage_player(
          4 * get_monster_dmg(monster->level, monster->exp_tier),
          DAMAGE_PHYSICAL
        );
      } else {
        sprintf(battle_post_message, str_monster2_dragon_legendary_tail_miss);
        SFX_MISS;
      }
      return;
    case LEGENDARY_WING_FLAP:
      sprintf(
        battle_pre_message, str_monster2_dragon_legendary_wing, monster->id);
      if (hit) {
        uint16_t wing_damage = damage_player(
          get_monster_dmg(monster->level, monster->exp_tier),
          DAMAGE_PHYSICAL
        );
        player.trip_turns = 1;
        sprintf(
          battle_post_message,
          str_monster2_dragon_legendary_wing_hit,
          wing_damage
        );
      } else {
        sprintf(battle_post_message, str_monster2_dragon_legendary_wing_miss);
        SFX_MISS;
      }
      return;
    }
  }

  if (
    dragon_fright(monster) &&
    move_roll < 2 &&
    !(player.debuffs & FLAG_DEBUFF_SCARED)
  ) {
    // Use fright action
    dragon_use_fright(monster);
    sprintf(battle_pre_message, str_monster2_dragon_fright, monster->id);
    bool scared = false;

    if (roll_attack_monster(monster->matk + 5, player.mdef)) {
      StatusEffectResult result = apply_scared(
        encounter.player_status_effects,
        A_TIER,
        2,
        player.debuff_immune
      );
      if (result == STATUS_RESULT_SUCCESS)
        scared = true;
    }

    if (scared) {
      sprintf(battle_post_message, str_monster2_dragon_fright_hit);
      SFX_MAGIC;
    }
    else {
      sprintf(battle_post_message, str_monster2_dragon_fright_miss);
      SFX_MISS;
    }

    return;
  }

  if (dragon_firebreath(monster) && move_roll < 2) {
    // Fire breath
    dragon_use_firebreath(monster);
    sprintf(battle_pre_message, str_monster2_dragon_fire_breath, monster->id);
    bool hit = roll_attack_monster(monster->matk, player.mdef);

    uint16_t base_damage = 3 * get_monster_dmg(
      level_offset(monster->level, 5),
      monster->exp_tier
    );

    if (!hit)
      base_damage /= 2;

    uint16_t damage = damage_player(base_damage, DAMAGE_FIRE);
    if (hit)
      return;

    sprintf(battle_post_message, str_monster2_dragon_fire_breath_miss, damage);
    SFX_MISS;

    return;
  } else if (d8() < 2) {
    dragon_restore_firebreath(monster);
  }

  // Multi-attack
  sprintf(battle_pre_message, str_monster2_dragon_attack, monster->id);
  uint16_t base_damage = get_monster_dmg(monster->level, monster->exp_tier);
  uint8_t hits = 0;
  for (uint8_t k = 0; k < 3; k++) {
    if (roll_attack_monster(monster->atk + 2, player.def))
      hits++;
  }

  if (hits == 0) {
    sprintf(battle_post_message, str_monster2_dragon_miss);
    SFX_MISS;
    return;
  }

  base_damage *= hits;
  uint16_t damage = damage_player(base_damage, DAMAGE_PHYSICAL);

  if (hits == 3)
    sprintf(battle_post_message, str_monster2_dragon_hit_triple, damage);
  else if (hits == 2)
    sprintf(battle_post_message, str_monster2_dragon_hit_double, damage);
  else
    sprintf(battle_post_message, str_monster2_dragon_hit_single, damage);

  SFX_MELEE;
}

void dragon_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_DRAGON, str_misc_dragon, &dragon_tileset, level, tier);

  m->palette = dragon_palettes + tier * 4;
  m->bank = BANK_7;
  m->take_turn = dragon_take_turn;

  m->exp_level = level_offset(level, 20);
  m->max_hp = get_monster_hp(level_offset(level, 20), tier);
  m->hp = m->max_hp;

  switch (m->exp_tier) {
  case C_TIER:
    init_dragon_parameter(m, false, 2, 1);
    break;
  case B_TIER:
    init_dragon_parameter(m, true, 2, 2);
    break;
  case A_TIER:
    init_dragon_parameter(m, true, 3, 2);
    break;
  case S_TIER:
    init_dragon_parameter(m, true, 3, 3);
    break;
  }

  monster_reset_stats(m);
}

