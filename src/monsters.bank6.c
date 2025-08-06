#pragma bank 6

#include <stdio.h>

#include "battle.h"
#include "battle.effects.h"
#include "data.h"
#include "monster.h"
#include "stats.h"
#include "strings.h"

// -----------------------------------------------------------------------------
// Monster 1 - Kobold
// -----------------------------------------------------------------------------

#define KOBOLD_PRONE_FLAG FLAG(7)

static void kobold_take_turn(Monster *m) {
  const uint8_t daze_chance[4] = { 13, 14, 15, 16 };
  const uint8_t prone_chance[4] = { 14, 15, 15, 17 };

  const uint8_t move_roll = d16();

  // Kobolds sometimes space out entirely
  if (move_roll >= daze_chance[m->exp_tier]) {
    sprintf(battle_pre_message, str_monster_kobold_dazed, m->id);
    SKIP_POST_MSG;
    SFX_FAIL;
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

  if (roll_attack_monster(atk, def)) {
    uint16_t base_damage = get_monster_dmg(m->level, m->exp_tier);
    clear_debug();
    damage_player(base_damage, type);
  } else if (d16() >= prone_chance[m->exp_tier]) {
    sprintf(battle_post_message, str_monster_kobold_miss);
    m->trip_turns = 1;
    SFX_FAIL;
  } else {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
  }
}

void kobold_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_KOBOLD, str_misc_kobold, &kobold_tileset, level, tier);

  m->palette = kobold_palettes + tier * 4;
  m->max_hp = get_monster_hp(level_offset(level, -2), tier);
  m->hp = m->max_hp;

  m->atk_base = get_monster_atk(level_offset(level, -3), tier);
  m->def_base = get_monster_def(level_offset(level, -2), tier);
  m->matk_base = get_monster_atk(level_offset(level, -1), tier);
  m->mdef_base = get_monster_def(level_offset(level, -4), tier);

  m->aspect_resist = DAMAGE_EARTH;
  m->aspect_vuln = DAMAGE_FIRE;

  m->bank = BANK_6;
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
    SKIP_POST_MSG;
    SFX_FAIL;
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

  if (!roll_attack_monster(atk, def)) {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
    return;
  }

  damage_player(get_monster_dmg(monster->level - 1, damage_tier), type);
}

void goblin_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_GOBLIN, str_misc_goblin, &goblin_tileset, level, tier);
  m->palette = goblin_palettes;

  m->palette = goblin_palettes + tier * 4;
  m->exp_level = level + 1;
  m->def_base = get_monster_def(level_offset(level, -2), tier);
  m->matk_base = get_monster_atk(level_offset(level, -1), tier);
  m->mdef_base = get_monster_def(level_offset(level, -3), tier);

  m->bank = BANK_6;
  m->take_turn = goblin_take_turn;

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
    if (roll_attack_monster(monster->atk, player.def)) {
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
      SFX_MAGIC;
    } else {
      sprintf(battle_post_message, str_monster_zombie_bite_miss, monster->id);
      SFX_FAIL;
    }
    return;
  }

  sprintf(battle_pre_message, str_monster_zombie_slam, monster->id);

  if (roll_attack_monster(monster->atk, player.def)) {
    damage_player(
      get_monster_dmg(monster->level, monster->exp_tier), DAMAGE_PHYSICAL);
  }
  else {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
  }
}

void zombie_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_ZOMBIE, str_misc_zombie, &zombie_tileset, level, tier);

  m->palette = zombie_palettes + tier * 4;
  m->exp_level = level + 2;

  m->max_hp = get_monster_hp(level_offset(level, 2), tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level_offset(level, -2), tier);
  m->def_base = get_monster_def(level_offset(level, -3), tier);
  m->matk_base = get_monster_atk(level_offset(level, -2), tier);
  m->mdef_base = get_monster_def(level_offset(level, -3), tier);
  m->agl_base = get_agl(level_offset(level, -5), tier);

  m->aspect_vuln = DAMAGE_FIRE | DAMAGE_MAGICAL;

  m->bank = BANK_6;
  m->take_turn = zombie_take_turn;

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
    if (roll_attack_monster(monster->atk, player.mdef)) {
      apply_scared(encounter.player_status_effects, C_TIER, 2, 0);
      sprintf(battle_post_message, str_monster_bugbear_for_hruggek_hit);
      monster->parameter--;
      SFX_MAGIC;
    } else {
      sprintf(battle_post_message, str_monster_bugbear_for_hruggek_miss);
      SFX_FAIL;
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

  if (roll_attack_monster(atk, player.def))
    damage_player(base_damage, DAMAGE_PHYSICAL);
  else {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
  }

}

void bugbear_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_BUGBEAR, str_misc_bugbear, &bugbear_tileset, level, tier);

  m->palette = bugbear_palettes + tier * 4;
  m->atk_base = get_monster_atk(level_offset(level, 1), tier);
  m->mdef_base = get_monster_def(level_offset(level, 2), tier);
  m->agl_base = get_agl(level_offset(level, 3), tier);

  m->debuff_immune = DEBUFF_BLIND | DEBUFF_CONFUSED | DEBUFF_POISONED;
  m->parameter = 1;

  m->bank = BANK_6;
  m->take_turn = bugbear_take_turn;

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
    if (roll_attack_monster(monster->atk, player.def)) {
      uint16_t base_damage = get_monster_dmg(monster->level, tier);
      if (d8() < 2) {
        // Toppled
        monster->parameter--;
        uint16_t dmg = damage_player(base_damage, DAMAGE_PHYSICAL);
        player.trip_turns = 1;
        sprintf(battle_post_message, str_monster_owlbear_pounce_topple, dmg);
        SFX_SPECIAL_CRIT;
      } else {
        damage_player(base_damage, DAMAGE_PHYSICAL);
      }
    } else {
      sprintf(battle_post_message, str_monster_owlbear_pounce_miss);
      SFX_FAIL;
    }
    return;
  }
  if (d8() < 2) {
    // Multi-attack
    sprintf(battle_pre_message, str_monster_owlbear_multi, monster->id);

    uint16_t damage = 0;
    if (roll_attack_monster(monster->atk, player.def))
      damage += get_monster_dmg(level_offset(monster->level, -2), tier);
    if (roll_attack_monster(monster->atk, player.def))
      damage += get_monster_dmg(monster->level, tier);

    if (damage > 0)
      damage_player(damage, DAMAGE_PHYSICAL);
    else {
      sprintf(battle_post_message, str_monster_miss);
      SFX_MISS;
    }
  } else {
    // Beak only
    sprintf(battle_pre_message, str_monster_owlbear_beak, monster->id);
    if (roll_attack_monster(monster->atk, player.def)) {
      damage_player(get_monster_dmg(
        level_offset(monster->level, -2), tier),
        DAMAGE_PHYSICAL);
    } else {
      sprintf(battle_post_message, str_monster_miss);
      SFX_MISS;
    }
  }
}

void owlbear_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_OWLBEAR, str_misc_owlbear, &owlbear_tileset, level, tier);
  m->palette = owlbear_palettes + tier * 4;
  m->exp_tier = tier;
  m->level = level + 2;

  m->max_hp = get_monster_hp(level_offset(level, 2), tier);
  m->hp = m->max_hp;
  m->mdef_base = get_monster_def(level_offset(level, -5), tier);
  m->agl_base = get_agl(level_offset(level, 3), tier);

  const uint8_t pounces[4] = { 1, 2, 3, 5 };
  m->parameter = pounces[tier];

  m->bank = BANK_6;
  m->take_turn = owlbear_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 6 - Gelatinous Cube
// -----------------------------------------------------------------------------

static void gelatinous_cube_take_turn(Monster *monster) {
  const uint8_t consume_chance[4] = { 2, 3, 4, 5 };

  if (d16() < 1) {
    // Search with feelers
    sprintf(battle_pre_message, str_monster_gcube_search, monster->id);
    SKIP_POST_MSG;
    SFX_FAIL;
    return;
  }

  if (d8() < consume_chance[monster->exp_tier] && monster->parameter > 0) {
    // Consume!
    monster->parameter--;

    const uint8_t paralyze_chance = monster->exp_tier > B_TIER ? 5 : 3;
    const uint8_t poison_chance = monster->exp_tier > B_TIER ? 8 : 5;

    if (d16() < paralyze_chance) {
      // Paralyze
      apply_paralyzed(
        encounter.player_status_effects, C_TIER, 2, player.debuff_immune);
      sprintf(battle_pre_message, str_monster_gcube_paralyze);
      SFX_MAGIC;
    } else if (d16() < poison_chance) {
      // Poison
      apply_poison(
        encounter.player_status_effects, C_TIER, 3, player.debuff_immune);
      sprintf(battle_pre_message, str_monster_gcube_poison);
      SFX_MAGIC;
    } else {
      sprintf(battle_pre_message, str_monster_gcube_engulf_fail, monster->id);
      SFX_FAIL;
    }
    SKIP_POST_MSG;
    return;
  }

  // Normal attack
  sprintf(battle_pre_message, str_monster_gcube_attack, monster->id);

  if (roll_attack_monster(monster->atk, player.def)) {
    PowerTier tier = monster->exp_tier;
    if (tier == A_TIER)
      tier = B_TIER;
    else if (tier == S_TIER)
      tier = A_TIER;
    uint16_t base_damage = get_monster_dmg(
      level_offset(monster->level, -5), tier);
    damage_player(base_damage, DAMAGE_PHYSICAL);
  } else {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
  }

  return;
}

void gelatinous_cube_generator(
  Monster *m,
  uint8_t level,
  PowerTier tier
) BANKED {
  monster_init_instance(
    m, MONSTER_GELATINOUS_CUBE,
    str_misc_gelatinous_cube, &gelatinous_cube_tileset, level, tier);
  m->palette = gelatinous_cube_palettes + tier * 4;
  m->exp_level = level + 3;

  m->atk_base = get_monster_atk(level_offset(level, 2), tier);
  m->mdef_base = get_monster_def(level_offset(level, -5), tier);

  m->aspect_resist = DAMAGE_PHYSICAL;
  m->aspect_vuln = DAMAGE_MAGICAL;
  m->debuff_immune = DEBUFF_POISONED | DEBUFF_BLIND | DEBUFF_SCARED;
  m->special_immune = SPECIAL_SLEET_STORM;

  // Number of times they can execute the "consume" ability
  m->parameter = tier < A_TIER ? 1 : 2;

  m->bank = BANK_6;
  m->take_turn = gelatinous_cube_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 7 - Displacer Beast
// -----------------------------------------------------------------------------

static void displacer_beast_take_turn(Monster *monster) {
  sprintf(
    battle_pre_message, str_monster_displacer_beast_tentacle, monster->id);

  bool hit1 = roll_attack_monster(monster->atk, player.def);
  bool hit2 = roll_attack_monster(level_offset(monster->atk, -7), player.def);

  uint8_t tier = monster->exp_tier > B_TIER ? A_TIER : C_TIER;

  uint16_t base_damage = get_monster_dmg(
    level_offset(monster->level, -10), tier);

  if (hit1 && hit2) {
    const uint16_t result = damage_player(2 * base_damage, DAMAGE_DARK);
    sprintf(battle_post_message, str_monster_displacer_beast_2hit, result);
    SFX_SPECIAL_CRIT;
  } else if (hit1 || hit2) {
    const uint16_t result = damage_player(base_damage, DAMAGE_DARK);
    sprintf(battle_post_message, str_monster_displacer_beast_1hit, result);
    SFX_MELEE;
  } else {
    sprintf(battle_post_message, str_monster_displacer_beast_miss);
    SFX_MISS;
  }
}

void displacer_beast_generator(
  Monster *m,
  uint8_t level,
  PowerTier tier
) BANKED {
  monster_init_instance(
    m, MONSTER_DISPLACER_BEAST,
    str_misc_displacer_beast, &displacer_beast_tileset, level, tier);
  m->palette = displacer_beast_palettes + tier * 4;

  m->exp_tier = tier;
  m->level = level + 2;

  m->max_hp = get_monster_hp(level_offset(level, 5), tier);
  m->hp = m->max_hp;
  m->def_base = get_monster_def(level_offset(level, 5), tier);
  m->mdef_base = get_monster_def(level_offset(level, 5), tier);

  m->aspect_vuln = DAMAGE_LIGHT;
  m->parameter = 2;

  m->bank = BANK_6;
  m->take_turn = displacer_beast_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 8 - Will-o-Wisp
// -----------------------------------------------------------------------------

static void will_o_wisp_take_turn(Monster *monster) {
  PowerTier tier = monster->exp_tier > B_TIER ? A_TIER : B_TIER;
  uint16_t base_damage = get_monster_dmg(monster->level, tier);

  // Life drain
  if (d8() < 2) {
    sprintf(battle_pre_message, str_monster_will_o_wisp_siphon, monster->id);
    if (roll_attack_monster(monster->matk, player.mdef)) {
      uint16_t damage = damage_player(base_damage / 2, DAMAGE_DARK);
      uint16_t heal = damage;
      if (damage + monster->target_hp > monster->max_hp)
        heal = monster->max_hp - monster->target_hp;
      monster->target_hp += heal;
      sprintf(battle_post_message, str_monster_will_o_wisp_siphon_hit, damage);
    } else {
      sprintf(battle_post_message, str_monster_miss);
      SFX_FAIL;
    }
    return;
  }

  // Phase terror!
  if (d8() < monster->parameter) {
    sprintf(battle_pre_message, str_monster_will_o_wisp_scare, monster->id);
    if (roll_attack_monster(monster->matk, level_offset(player.mdef, -5))) {
      const uint8_t scared_turns[4] = { 2, 3, 5, 7 };
      apply_scared(encounter.player_status_effects,
        A_TIER, scared_turns[monster->exp_tier], player.debuff_immune);
      sprintf(battle_post_message, str_monster_will_o_wisp_scare_hit);
      SFX_MAGIC;
    } else {
      sprintf(battle_post_message, str_monster_will_o_wisp_scare_miss);
      SFX_FAIL;
    }
    return;
  }

  // Normal attack
  sprintf(battle_pre_message, str_monster_will_o_wisp_lightning, monster->id);
  if (roll_attack_monster(monster->matk, player.mdef)) {
    damage_player(base_damage, DAMAGE_AIR);
  } else {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
  }
}

void will_o_wisp_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_WILL_O_WISP, str_misc_will_o_wisp, &will_o_wisp_tileset,
    level, tier);
  m->palette = will_o_wisp_palettes + tier * 4;

  m->exp_tier = tier;
  m->level = level;

  m->max_hp = get_monster_hp(level_offset(level, -10), tier);
  m->hp = m->max_hp;
  m->def_base = get_monster_def(level_offset(level, 5), tier);

  m->aspect_resist = DAMAGE_PHYSICAL;
  m->aspect_vuln = DAMAGE_LIGHT;
  m->debuff_immune = DAMAGE_DARK;

  const uint8_t phase_terror_chance[4] = { 1, 2, 3, 4 };
  m->parameter = phase_terror_chance[tier];

  m->bank = BANK_6;
  m->take_turn = will_o_wisp_take_turn;

  monster_reset_stats(m);
}

// -----------------------------------------------------------------------------
// Monster 9 - Death Knight
// -----------------------------------------------------------------------------

static void deathknight_take_turn(Monster *monster) {
  const PowerTier tier = monster->exp_tier > B_TIER ? A_TIER : B_TIER;
  const uint8_t longsword_level = level_offset(monster->level, -2);
  const uint8_t orb_level = level_offset(monster->level, 4);

  uint16_t base_damage;

  if (!(monster->parameter & DEATH_KNIGHT_ORB_USED) && d8() < 1) {
    // Hellfire orb
    monster->parameter |= DEATH_KNIGHT_ORB_USED;
    sprintf(battle_pre_message,
      str_monster_deathknight_hellfire, monster->id);

    base_damage = get_monster_dmg(orb_level, tier);
    if (roll_attack_monster(monster->matk, player.mdef)) {
      uint16_t damage = damage_player(base_damage, DAMAGE_FIRE);
      sprintf(battle_post_message,
        str_monster_deathknight_hellfire_hit, damage);
    } else {
      uint16_t damage = damage_player(base_damage / 2, DAMAGE_FIRE);
      sprintf(battle_post_message,
        str_monster_deathknight_hellfire_miss, damage);
    }
    return;
  }

  // Longsword multi-attack
  sprintf(battle_pre_message, str_monster_deathknight_attack, monster->id);

  bool hit1 = roll_attack_monster(monster->atk, player.def);
  bool hit2 = roll_attack_monster(monster->atk, player.def);

  base_damage = get_monster_dmg(longsword_level, tier);

  if (hit1 && hit2) {
    base_damage *= 2;
    uint16_t damage = damage_player(base_damage, DAMAGE_PHYSICAL);
    sprintf(battle_post_message, str_monster_deathknight_hit2, damage);
  } else if (hit1 || hit2) {
    uint16_t damage = damage_player(base_damage, DAMAGE_PHYSICAL);
    sprintf(battle_post_message, str_monster_deathknight_hit1, damage);
  } else {
    sprintf(battle_post_message, str_monster_miss);
    SFX_MISS;
  }
}

void deathknight_generator(Monster *m, uint8_t level, PowerTier tier) BANKED {
  monster_init_instance(
    m, MONSTER_DEATHKNIGHT, str_misc_death_knight, &deathknight_tileset,
    level, tier);
  m->palette = deathknight_palettes + tier * 4;
  m->exp_level = level + 5;

  m->max_hp = get_monster_hp(level_offset(level, 5), tier);
  m->hp = m->max_hp;
  m->atk_base = get_monster_atk(level_offset(level, 2), tier);
  m->def_base = get_monster_def(level_offset(level, 5), tier);
  m->matk_base = get_monster_atk(level_offset(level, 2), tier);

  m->aspect_resist = DAMAGE_MAGICAL;
  m->aspect_vuln = DAMAGE_LIGHT;
  m->debuff_immune = DAMAGE_DARK;

  m->bank = BANK_6;
  m->take_turn = deathknight_take_turn;

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
    if (roll_attack_monster(dummy->atk, player.def)) {
      const uint16_t base = get_monster_dmg(dummy->level, dummy->exp_tier);
      damage_player(base, DAMAGE_PHYSICAL);
    } else {
      sprintf(battle_post_message, str_monster_miss);
    }
    break;
  default:
    SKIP_POST_MSG;
    break;
  }
}

void dummy_generator(Monster *m, uint8_t level, TestDummyType type) BANKED {
  PowerTier tier = C_TIER;

  monster_init_instance(
    m,
    MONSTER_DUMMY,
    str_misc_dummy,
    &test_dummy_tileset,
    level,
    tier
  );

  m->palette = dummy_palette;
  m->parameter = type;
  m->take_turn = dummy_take_turn;

  monster_reset_stats(m);
}
