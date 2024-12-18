#pragma bank 3

#include <stdio.h>
#include "encounter.h"

/**
 * Rolls initiative for all active entities in the fight and sets the turn
 * order for actions.
 */
void roll_initiative(void) {
  uint8_t rolls[5] = { 0, 0, 0, 0, 0 };

  // Reset the turn order
  encounter.turn_index = 0;
  encounter.turn = TURN_PLAYER;
  encounter.round_complete = false;
  encounter.order[0] = TURN_PLAYER;
  encounter.order[1] = TURN_END;
  encounter.order[2] = TURN_END;
  encounter.order[3] = TURN_END;
  encounter.order[4] = TURN_END;

  // Roll for the player
  rolls[0] = d32() + player.agl + 1;

  // Roll for the monsters that currently live
  MonsterInstance *mon = encounter.monsters;
  for (uint8_t m = 1; m < 4; m++) {
    if (mon->active) {
      encounter.order[m] = TURN_PLAYER + m;
      rolls[m] = d32() + 1 + mon->agl;
    }
    mon++;
  }

  // When's the last time you wrote a bespoke bubble sort? ;)
  for (uint8_t k = 0; k < 4; k++) {
    for (uint8_t j = k; j < 4; j++) {
      if (rolls[k] < rolls[j]) {
        uint8_t tmp = rolls[k];
        rolls[k] = rolls[j];
        rolls[j] = tmp;
        tmp = encounter.order[k];
        encounter.order[k] = encounter.order[j];
        encounter.order[j] = tmp;
      }
    }
  }
}

void next_turn(void) {
  do {
    encounter.turn = encounter.order[encounter.turn_index++];
    switch (encounter.turn) {
    case TURN_END:
      encounter.round_complete = true;
      return;
    case TURN_PLAYER:
      return;
    case TURN_MONSTER1:
      if (encounter.monsters->active)
        return;
      break;
    case TURN_MONSTER2:
      if ((encounter.monsters + 1)->active)
        return;
      break;
    case TURN_MONSTER3:
      if ((encounter.monsters + 2)->active)
        return;
      break;
    }
  } while (encounter.turn_index < 5);
}

void check_status_effects(void) {
  // TODO Update status effects & execute rendering callbacks
  switch (encounter.turn) {
  case TURN_PLAYER:
    reset_player_stats();
    break;
  case TURN_MONSTER1:
    monster_reset_stats(encounter.monsters);
    break;
  case TURN_MONSTER2:
    monster_reset_stats(encounter.monsters + 1);
    break;
  case TURN_MONSTER3:
    monster_reset_stats(encounter.monsters + 2);
    break;
  }
}

inline void player_turn(void) {
  switch (encounter.player_action) {
  case PLAYER_ACTION_FIGHT:
    player_base_attack();
    break;
  case PLAYER_ACTION_ABILITY:
    // No sp bounds checking here, should be handled in battle UI
    player.sp -= encounter.player_ability->sp_cost;
    encounter.player_ability->execute();
    break;
  case PLAYER_ACTION_ITEM:
    break;
  case PLAYER_ACTION_FLEE:
    player_flee();
    break;
  }
}

inline void monster_turn(void) {
  const uint8_t offset = encounter.turn - TURN_MONSTER1;
  MonsterInstance *monster = encounter.monsters + offset;
  if (monster->active)
    monster->take_turn(monster);
}

void take_action(void) {
  if (encounter.turn == TURN_PLAYER)
    player_turn();
  else
    monster_turn();

  MonsterInstance *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++) {
    if (monster->target_hp != monster->hp)
      monster->hp_delta = monster->target_hp - monster->hp;
  }
}

void set_player_fight(MonsterInstance *target) {
  encounter.player_action = PLAYER_ACTION_FIGHT;
  encounter.target = target;
}

void set_player_ability(const Ability *a, MonsterInstance *target) {
  encounter.player_action = PLAYER_ACTION_ABILITY;
  encounter.player_ability = a;
  encounter.target = target;
}

void set_player_flee(void) {
  encounter.player_action = PLAYER_ACTION_FLEE;
}

bool monsters_slain(void) {
  MonsterInstance *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++) {
    if (monster->active)
      return false;
  }
  return true;
}

void reset_encounter(MonsterLayout layout) NONBANKED {
  encounter.layout = layout;
  for (uint8_t k = 0; k < 5; k++) {
    encounter.order[k] = TURN_END;
  }
  encounter.turn = TURN_END;
  encounter.turn_index = 0;
  encounter.round_complete = 0;
  encounter.player_action = PLAYER_ACTION_FIGHT;
  encounter.player_ability = NULL;
  encounter.target = NULL;
  encounter.xp_reward = 0;

  for (uint8_t eff = 0; eff < STATUS_EFFECTS; eff++) {
    encounter.player_status_effects[eff].active = false;
    encounter.player_status_effects[eff].duration = 0;
    encounter.player_status_effects[eff].tier = C_TIER;
  }

  MonsterInstance *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++) {
    for (uint8_t eff = 0; eff < STATUS_EFFECTS; eff++) {
      monster->status_effects[eff].active = false;
      monster->status_effects[eff].duration = 0;
      monster->status_effects[eff].tier = C_TIER;
    }
    monster_deactivate(monster++);
  }
}

void damage_monster(uint16_t base_damage, DamageAspect type) {
  MonsterInstance *monster = encounter.target;

  if (!monster)
    return;

  if (monster->aspect_immune & type) {
    sprintf(battle_post_message, str_battle_player_hit_immune);
    return;
  }

  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);
  bool critical = is_critical(roll);

  if (critical) {
    sprintf(battle_post_message, str_battle_player_hit_crit, damage);
  } else if (monster->aspect_resist & type) {
    damage >>= 1;
    sprintf(battle_post_message, str_battle_player_hit_resist, damage);
  } else if (monster->aspect_vuln & type) {
    damage <<= 1;
  } else {
    sprintf(battle_post_message, str_battle_player_hit, damage);
  }

  if (monster->target_hp < damage)
    monster->target_hp = 0;
  else
    monster->target_hp -= damage;
}

void damage_all_monster(uint16_t base_damage, DamageAspect type) {
  uint8_t roll = d16();
  uint16_t damage = calc_damage(roll, base_damage);

  MonsterInstance *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++, monster++) {
    if (!monster->active)
      continue;

    if (monster->aspect_immune & type)
      continue;

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
}

void ability_placeholder(void) {
  sprintf(battle_pre_message, "You try a thing.");
  sprintf(battle_post_message, "It doesn't work.");
}

Encounter encounter = {
  MONSTER_LAYOUT_1,
  { {NULL}, {NULL}, {NULL} },
  { TURN_END, TURN_END, TURN_END, TURN_END, TURN_END },
  false,
  0,
  TURN_PLAYER,
  PLAYER_ACTION_FIGHT,
  NULL,
  NULL,
  0
};
