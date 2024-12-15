#pragma bank 3

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
  uint8_t player_agl = get_agl(player.level, player.summon->agl_tier);
  rolls[0] = d32() + player_agl + 1;

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
    player.summon->base_attack(encounter.target);
    break;
  case PLAYER_ACTION_FLEE:
    break;
  case PLAYER_ACTION_ABILITY:
    break;
  case PLAYER_ACTION_SUMMON:
    break;
  case PLAYER_ACTION_ITEM:
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
}

void set_player_fight(MonsterInstance *target) {
  encounter.player_action = PLAYER_ACTION_FIGHT;
  encounter.target = target;
}

void set_player_ability(Ability *a, MonsterInstance *target) {
  encounter.player_action = PLAYER_ACTION_ABILITY;
  encounter.player_ability = a;
  encounter.target = target;
}

void set_player_summon(Summon *summon) {
  encounter.player_action = PLAYER_ACTION_SUMMON;
}

void set_player_flee(void) {
  encounter.player_action = PLAYER_ACTION_FLEE;
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
