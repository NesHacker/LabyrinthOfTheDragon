#pragma bank 3

#include <stdio.h>
#include "encounter.h"

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

/**
 * Resets the update flags for a set of status effects.
 * @param effect Pointer the list of effects.
 */
inline void reset_effects_flags(StatusEffectInstance *effect) {
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++)
    effect->update = false;
}

/**
 * Updates a status effect duration and determines if it is still active.
 * @param effect Status effect to update.
 */
inline void update_effect_duration(StatusEffectInstance *effect) {
  if (!effect->active)
    return;

  // Perpetual effects never end.
  if (effect->duration == EFFECT_DURATION_PERPETUAL)
    return;

  if (effect->duration == 0) {
    effect->active = false;
    effect->update = true;
  }

  effect->duration--;
}

void reset_player_stats(void) NONBANKED {
  player.atk = player.atk_base;
  player.def = player.def_base;
  player.matk = player.matk_base;
  player.mdef = player.mdef_base;
  player.target_hp = player.hp;
  reset_effects_flags(encounter.player_status_effects);
}

void monster_reset_stats(MonsterInstance *m) NONBANKED {
  m->agl = m->agl_base;
  m->atk = m->atk_base;
  m->def = m->def_base;
  m->matk = m->matk_base;
  m->mdef = m->mdef_base;
  m->target_hp = m->hp;
  m->hp_delta = 0;
  reset_effects_flags(m->status_effects);
}

void update_player_status_effects(void) {
  StatusEffectInstance *effect = encounter.player_status_effects;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
    update_effect_duration(effect);
    if (!effect->active)
      continue;

    switch (k) {
    case DEBUFF_BLIND:
      player.atk = blind_atk(player.atk_base, effect->tier);
      break;
    case DEBUFF_AGL_DOWN:
      player.agl = agl_down(player.agl_base, effect->tier);
      break;
    case DEBUFF_ATK_DOWN:
      player.atk = atk_down(player.atk_base, effect->tier);
      break;
    case DEBUFF_DEF_DOWN:
      player.def = def_down(player.def_base, effect->tier);
      break;
    case BUFF_AGL_UP:
      player.agl = agl_up(player.agl_base, effect->tier);
      break;
    case BUFF_ATK_UP:
      player.atk = atk_up(player.atk_base, effect->tier);
      break;
    case BUFF_DEF_UP:
      player.def = def_up(player.def_base, effect->tier);
      break;
    /*
    // TODO Handle these debuffs (as they come in game design)
    case BUFF_HASTE: break;
    case DEBUFF_POISONED: break;
    case BUFF_REGEN: break;
    case DEBUFF_SCARED: break;
    case DEBUFF_PARALZYED: break;
    case DEBUFF_CONFUSED: break;
    */
    }
  }
}

void update_monster_status_effects(MonsterInstance *monster) {
  StatusEffectInstance *effect = monster->status_effects;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
    update_effect_duration(effect);
    if (!effect->active)
      continue;

    switch (k) {
    case DEBUFF_BLIND:
      monster->atk = blind_atk(monster->atk, effect->tier);
      break;
    case DEBUFF_AGL_DOWN:
      monster->agl = agl_down(monster->agl_base, effect->tier);
      break;
    case DEBUFF_ATK_DOWN:
      monster->atk = atk_down(monster->atk_base, effect->tier);
      break;
    case DEBUFF_DEF_DOWN:
      monster->def = def_down(monster->def_base, effect->tier);
      break;
    case BUFF_AGL_UP:
      monster->agl = agl_up(monster->agl_base, effect->tier);
      break;
    case BUFF_ATK_UP:
      monster->atk = atk_up(monster->atk_base, effect->tier);
      break;
    case BUFF_DEF_UP:
      monster->def = def_up(monster->def_base, effect->tier);
      break;
    /*
    // TODO Handle these debuffs (as they come in game design)
    case BUFF_HASTE: break;
    case DEBUFF_POISONED: break;
    case BUFF_REGEN: break;
    case DEBUFF_SCARED: break;
    case DEBUFF_PARALZYED: break;
    case DEBUFF_CONFUSED: break;
    */
    }
  }
}

void check_status_effects(void) {
  switch (encounter.turn) {
  case TURN_PLAYER:
    reset_player_stats();
    update_player_status_effects();
    break;
  case TURN_MONSTER1:
    monster_reset_stats(encounter.monsters);
    update_monster_status_effects(encounter.monsters);
    break;
  case TURN_MONSTER2:
    monster_reset_stats(encounter.monsters + 1);
    update_monster_status_effects(encounter.monsters + 1);
    break;
  case TURN_MONSTER3:
    monster_reset_stats(encounter.monsters + 2);
    update_monster_status_effects(encounter.monsters + 2);
    break;
  }
}

inline void player_turn(void) {
    // // Handled in the turn function
    // case DEBUFF_SCARED: break;
    // case DEBUFF_PARALZYED: break;
    // case DEBUFF_POISONED: break;
    // case DEBUFF_CONFUSED: break;
    // case BUFF_REGEN: break;

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

  if (!monster->active)
    return;


    // // Handled in the turn function
    // case DEBUFF_SCARED: break;
    // case DEBUFF_PARALZYED: break;
    // case DEBUFF_POISONED: break;
    // case DEBUFF_CONFUSED: break;
    // case BUFF_REGEN: break;

  // // Check "scared" debuff
  // StatusEffectInstance *effect;
  // effect = get_effect(monster->status_effects, DEBUFF_SCARED);
  // if (effect->active) {
  //   if (
  //     monster->can_flee &&
  //     calc_scared_flee(effect->tier) &&
  //     monster_flee(monster)
  //   ) {
  //     return;
  //   }

  //   if (calc_scared_frozen(effect->tier)) {
  //     return;
  //   }
  // }

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

/**
 * Resets a list of status effects.
 * @param effect List of status effects to reset.
 */
inline void reset_status_effects(StatusEffectInstance *effect) {
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
    effect->active = false;
    effect->update = false;
    effect->duration = 0;
    effect->tier = C_TIER;
    effect->effect = NO_STATUS_EFFECT;
  }
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
  reset_status_effects(encounter.player_status_effects);

  MonsterInstance *monster = encounter.monsters;
  for (uint8_t k = 0; k < 3; k++, monster++) {
    reset_status_effects(monster->status_effects);
    monster_deactivate(monster);
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

StatusEffectResult apply_status_effect(
  StatusEffectInstance *list,
  StatusEffect effect,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  StatusEffectInstance *e = list;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, e++) {
    if (e->active && e->effect == effect)
      return STATUS_RESULT_DUPLICATE;
  }

  e = list;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, e++) {
    if (e->active)
      continue;
    if (is_debuff(effect) && is_debuff_immmune(immune, effect))
      return STATUS_RESULT_IMMUNE;
    e->effect = effect;
    e->active = true;
    e->update = true;
    e->tier = tier;
    e->duration = duration;
    return STATUS_RESULT_SUCCESS;
  }

  return STATUS_RESULT_MAX;
}

bool player_flee(void) {
  // TODO Implement me
  return false;
}

bool monster_flee(MonsterInstance *monster) {
  // TODO Implement me
  return false;
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
