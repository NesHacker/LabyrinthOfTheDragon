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

  MonsterInstance *target = encounter.target;
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

const Ability druid0 = {
  1, str_ability_druid_cure_wounds,
  TARGET_SELF, 4, druid_cure_wounds,
};

const Ability druid1 = {
  2, str_ability_druid_bark_skin,
  TARGET_SELF, 12, druid_bark_skin,
};

const Ability druid2 = {
  3, str_ability_druid_ligtning,
  TARGET_SINGLE, 18, druid_lightning,
};

const Ability druid3 = {
   4, str_ability_druid_heal,
   TARGET_SELF, 20, druid_heal,
};

const Ability druid4 = {
   5, str_ability_druid_insect_plague,
   TARGET_ALL, 45, druid_insect_plague,
};

const Ability druid5 = {
   6, str_ability_druid_regen,
   TARGET_SELF, 30, druid_regen
};


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

const Ability fighter0 = { 1 };
const Ability fighter1 = { 2 };
const Ability fighter2 = { 3 };
const Ability fighter3 = { 4 };
const Ability fighter4 = { 5 };
const Ability fighter5 = { 6 };

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

const Ability monk0 = { 1 };
const Ability monk1 = { 2 };
const Ability monk2 = { 3 };
const Ability monk3 = { 4 };
const Ability monk4 = { 5 };
const Ability monk5 = { 6 };

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

const Ability sorcerer0 = { 1 };
const Ability sorcerer1 = { 2 };
const Ability sorcerer2 = { 3 };
const Ability sorcerer3 = { 4 };
const Ability sorcerer4 = { 5 };
const Ability sorcerer5 = { 6 };

//------------------------------------------------------------------------------
// Class: Test Class
//------------------------------------------------------------------------------
// Use this to test various combat abilities without modifying the core classes.
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
  ability_placeholder();
}

void test_class_ability2(void) {
  ability_placeholder();
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

const Ability test_class0 = {
  1, "Damage All", TARGET_SELF, 0, test_class_ability0
};

const Ability test_class1 = {
  2, "Test 2", TARGET_SELF, 0, test_class_ability1
};

const Ability test_class2 = {
  3, "Test 3", TARGET_SELF, 0, test_class_ability2
};

const Ability test_class3 = {
  4, "Test 4", TARGET_SELF, 0, test_class_ability3
};

const Ability test_class4 = {
  5, "Test 5", TARGET_SELF, 0, test_class_ability4
};

const Ability test_class5 = {
  6, "Test 6", TARGET_SELF, 0, test_class_ability5
};

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

void set_player_abilities(void) {
  uint8_t flags = player.ability_flags;
  player_num_abilities = 0;

  for (uint8_t k = 0; k < MAX_ABILITIES; k++, flags >>= 1) {
    player_abilities[player_num_abilities] = &null_ability;
    if (flags & 1)
      player_abilities[player_num_abilities++] = class_abilities[k];
  }
}

void init_player(PlayerClass player_class) {
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

void init_test_player(uint8_t level) {
  init_player(CLASS_TEST);
  grant_ability(ABILITY_ALL);
  set_player_level(level);
  sprintf(player.name, "Tester");
  player.message_speed = AUTO_PAGE_FAST;
}

void grant_ability(AbilityFlag flag) {
  player.ability_flags |= flag;
  set_player_abilities();
}

void full_heal_player(void) {
  player.hp = player.max_hp;
  player.sp = player.max_sp;
}

void set_player_level(uint8_t level) {
  player.level = level;
  player.exp = get_exp(player.level);
  player.next_level_exp = get_exp(player.level + 1);
  update_player_stats();
  full_heal_player();
}

void player_base_attack(void) {
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
  }
}
