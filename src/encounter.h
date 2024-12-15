#ifndef _ENCOUNTER_H
#define _ENCOUNTER_H

#include <stdint.h>
#include <stdbool.h>

#include "battle.h"
#include "data.h"
#include "monster.h"
#include "stats.h"
#include "player.h"

/**
 * Result of the damage in terms of aspect immunities, resistances, and
 * vulnerabilities.
 */
typedef enum DamageResult {
  DAMAGE_RESULT_NORMAL,
  DAMAGE_RESULT_IMMUNE,
  DAMAGE_RESULT_RESIST,
  DAMAGE_RESULT_VULN,
} DamageResult;

/**
 * Enumerates the four possible monster layouts for a battle.
 */
typedef enum MonsterLayout {
  /**
   * One monster of any type (Sm, Md, Lg).
   */
  MONSTER_LAYOUT_1,
  /**
   * Two monsters of any type (Sm, Md, Lg).
   */
  MONSTER_LAYOUT_2,
  /**
   * Three small monsters.
   */
  MONSTER_LAYOUT_3S,
  /**
   * One medium and two small monsters.
   */
  MONSTER_LAYOUT_1M_2S,
} MonsterLayout;

/**
 * Denotes an action that can be taken by the player during battle.
 */
typedef enum PlayerAction {
  PLAYER_ACTION_FIGHT,
  PLAYER_ACTION_ABILITY,
  PLAYER_ACTION_ITEM,
  PLAYER_ACTION_SUMMON,
  PLAYER_ACTION_FLEE
} PlayerAction;

/**
 * Used to indicate the turn order during an encounter.
 */
typedef enum Turn {
  TURN_END,
  TURN_PLAYER,
  TURN_MONSTER1,
  TURN_MONSTER2,
  TURN_MONSTER3,
} Turn;

/**
 * Data structure detailing the current encounter.
 */
typedef struct Encounter {
  /**
   * Monster layout to use for the battle.
   */
  MonsterLayout layout;
  /**
   * Monster instances for the encounter.
   */
  MonsterInstance monsters[3];
  /**
   * The turn order for the encounter.
   */
  Turn order[5];
  /**
   * Whether or not the round is complete.
   */
  bool round_complete;
  /**
   * Index for who's turn it currently is.
   */
  uint8_t turn_index;
  /**
   * Whos turn it is.
   */
  Turn turn;
  /**
   * Dentoes what action the player chose to take on their turn.
   */
  PlayerAction player_action;
  /**
   * If the player took the "summon action", this is the summon they chose.
   */
  Summon *summon;
  /**
   * If the player used an ability, this is the ability they chose.
   */
  Ability *player_ability;
  /**
   * If the player is targeting a monster, this is the index for that monster.
   */
  MonsterInstance *target;
} Encounter;

/**
 * Encounter controller. Handles turn based combat logic for the game.
 * @see battle.c For battle rendering and effects.
 */
extern Encounter encounter;

/**
 * Sets the player's next action to a basic attack.
 * @param target Target for the basic attack.
 */
void set_player_fight(MonsterInstance *target);

/**
 * Sets the player's next action to using an ability.
 * @param a Ability to use.
 * @param target Target for the ability (if applicable).
 */
void set_player_ability(Ability *a, MonsterInstance *target);

/**
 * Sets the player's next action to changing the equipped summon.
 * @param summon New summon to equip.
 */
void set_player_summon(Summon *summon);

/**
 * Sets the player's next action to fleeing.
 */
void set_player_flee(void);

// void set_player_item(Item *item);

/**
 * Rolls initiative for the current combat round.
 */
void roll_initiative(void);

/**
 * Determines whos turn it is and initializes them.
 */
void next_turn(void);

/**
 * Updates status effects and updates current stats.
 */
void check_status_effects(void);

/**
 * Performs the action for the turn.
 */
void take_action(void);

/**
 * @return The monster at the given index.
 * @param idx Index for the monster in the battle.
 */
inline MonsterInstance *get_monster(uint8_t idx) {
  return encounter.monsters + idx;
}


#endif
