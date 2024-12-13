#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdbool.h>

#include "battle.h"
#include "player.h"
#include "monster.h"
#include "tables.h"
#include "util.h"

Player player = {
  CLASS_DRUID,
};

Timer walk_timer;
uint8_t walk_frame;

void init_player(void) {
  player.level = 10;
  player.exp = get_exp(10) + 23;
  player.next_level_exp = get_exp(11);

  player.hp = player.max_hp = get_player_hp(10, B_TIER);
  player.sp = player.max_sp = get_player_sp(10, A_TIER);
  player.atk = player.atk_base = get_player_atk(10, C_TIER);
  player.def = player.def_base = get_player_def(10, B_TIER);
  player.matk = player.matk_base = get_player_atk(10, A_TIER);
  player.mdef = player.mdef_base = get_player_def(17, B_TIER);
  player.summon = &summon_kobold;
  player.default_summon = &summon_kobold;
}

//------------------------------------------------------------------------------
// Summon 1 - Wilbur the Kobold
//------------------------------------------------------------------------------

/**
 * Activation method for the kobold.
 * @see `Summon.activate`
 */
void kobold_activate(void) {
  switch (player.player_class) {
  case CLASS_DRUID:
    summon_kobold.ability = &kobold_vine_whip;
    break;
  // case CLASS_FIGHTER:
  //   break;
  // case CLASS_MONK:
  //   break;
  // case CLASS_SORCERER:
  //   break;
  }
}

/**
 * Base attack ability execution function for the kobold.
 * @see `Summon.base_attack`
 */
void kobold_base_attack(void) {
}

Summon summon_kobold = {
  1,
  "Kobold",
  "Wilbur",
  B_TIER,
  EMPTY_BITFIELD,
  DAMAGE_EARTH,
  DAMAGE_LIGHT,
  EMPTY_BITFIELD,
  kobold_activate,
  kobold_base_attack,
};

void execute_kobold_vine_whip(void) {
}

Ability kobold_vine_whip = {
  "Vine Whip",
  CLASS_DRUID,
  C_TIER,
  3,
  TARGET_SINGLE,
  2,
  { FONT_SPACE, FONT_SPACE, FONT_DIGIT(2) },
  execute_kobold_vine_whip,
  &kobold_mend // next
};

void execute_kobold_mend(void) {
}

Ability kobold_mend = {
  "Mend",
  CLASS_DRUID,
  C_TIER,
  5,
  TARGET_SELF,
  5,
  { FONT_SPACE, FONT_SPACE, FONT_DIGIT(5) },
  execute_kobold_mend,
  &kobold_summon_ants,
};

void execute_kobold_summon_ants(void) {
}

Ability kobold_summon_ants = {
  "Summon Ants",
  CLASS_DRUID,
  B_TIER,
  9,
  TARGET_SINGLE,
  9,
  { FONT_SPACE, FONT_SPACE, FONT_DIGIT(9) },
  execute_kobold_summon_ants,
  &kobold_dirty_fang,
};

void execute_kobold_dirty_fang(void) {
}

Ability kobold_dirty_fang = {
  "Dirty Fang",
  CLASS_DRUID,
  B_TIER,
  14,
  TARGET_SINGLE,
  12,
  { FONT_SPACE, FONT_DIGIT(1), FONT_DIGIT(2) },
  execute_kobold_dirty_fang,
  NULL
};
