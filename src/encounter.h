#ifndef _ENCOUNTER_H
#define _ENCOUNTER_H

#include <stdint.h>
#include <stdbool.h>

#include "battle.h"
#include "data.h"
#include "item.h"
#include "monster.h"
#include "stats.h"
#include "player.h"

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
  PLAYER_NO_ACTION,
  PLAYER_ACTION_FIGHT,
  PLAYER_ACTION_ABILITY,
  PLAYER_ACTION_ITEM,
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
 * Encounter table entry for the random encounter system.
 */
typedef struct EncounterTable {
  /**
   * Odds out of 256 that this encounter will be chosen.
   */
  Odds odds;
  /**
   * Encounter layout.
   */
  MonsterLayout layout;
  /**
   * Monster type for the first monster.
   */
  MonsterType monster1;
  /**
   * Level for the first monster.
   */
  uint8_t monster1_level;
  /**
   * Tier for the first monster.
   */
  PowerTier monster1_tier;
  /**
   * Monster type for the second monster.
   */
  MonsterType monster2;
  /**
   * Level for the second monster.
   */
  uint8_t monster2_level;
  /**
   * Power tier for the second monster.
   */
  PowerTier monster2_tier;
  /**
   * Type for the third monster.
   */
  MonsterType monster3;
  /**
   * Level for the third monster.
   */
  uint8_t monster3_level;
  /**
   * Tier for the third monster.
   */
  PowerTier monster3_tier;
} EncounterTable;

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
  Monster monsters[3];
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
   * If the player used an ability, this is the ability they chose.
   */
  const Ability *player_ability;
  /**
   * If the player used an item, this is the id of the item they used.
   */
  ItemId item_id;
  /**
   * If the player is targeting a monster, this is the index for that monster.
   */
  Monster *target;
  /**
   * Accumulated xp to reward upon finishing battle.
   */
  uint16_t xp_reward;
  /**
   * Active player status effects.
   */
  StatusEffectInstance player_status_effects[MAX_ACTIVE_EFFECTS];
  /**
   * Set to `true` if the player successfully fled from combat.
   */
  bool player_fled;
  /**
   * Set to `true` if the player died as of the last action.
   */
  bool player_died;
  /**
   * Whether or not the encounter is over and the player has won.
   */
  bool victory;
  /**
   * Used to denote that this encounter is being used during testing. The battle
   * system will not return to the map after the battle is complete.
   */
  bool is_test;
  /**
   * Denotes that this is the final boss of the game.
   */
  bool is_final_boss;
  /**
   * Callback to execute upon encounter success. This callback is reset after
   * being called.
   */
  void (*on_victory)(void) BANKED;
} Encounter;

/**
 * Encounter controller. Handles turn based combat logic for the game.
 * @see battle.c For battle rendering and effects.
 */
extern Encounter encounter;

/**
 * Set to `true` during testing to disable the random encounter system.
 */
extern bool disable_encounters;

/**
 * Sets the victory callback for the encounter.
 * @param callback Victory callback to execute if the player is victorious.
 */
inline void set_on_victory(void (*callback)(void) BANKED) {
  encounter.on_victory = callback;
}

/**
 * Sets parameters for random encounter checking.
 * @param s Number of safe steps before encounter rolls begin.
 * @param ic Initial chance for an encounter to occur.
 * @param i Chance increment per each step if an encounter doesn't occur.
 * @param ts Whether or not the torch keeps the player safe.
 */
void config_random_encounter(uint8_t s, uint8_t ic, uint8_t i, bool ts);

/**
 * Determines if a random encounter has occured.
 * @return `true` If a random encounter has occured.
 */
bool check_random_encounter(void);

/**
 * Generates a random encounter from the given encounter table.
 * @param table Table to use when generating the encounter.
 */
void generate_encounter(EncounterTable *table) NONBANKED;

/**
 * Sets the player's next action to a basic attack.
 * @param target Target for the basic attack.
 */
void set_player_fight(Monster *target);

/**
 * Sets the player's next action to using an ability.
 * @param a Ability to use.
 * @param target Target for the ability (if applicable).
 */
void set_player_ability(const Ability *a, Monster *target);

/**
 * Sets the player's next action to fleeing.
 */
void set_player_flee(void);

/**
 * Sets the player's next action to using an item.
 */
void set_player_item(ItemId item_id);

/**
 * Called before each round of combat to initialize the encounter state.
 */
void before_round(void);

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
 * Call after an action completes to perform any state management before the
 * next action is taken.
 */
void after_action(void);

/**
 * @return `true` If all the monsters have been defeated.
 */
bool is_battle_over(void);

/**
 * Resets the encounter so it can be reconfigured for another battle.
 */
void reset_encounter(MonsterLayout layout);

/**
 * @return The monster at the given index.
 * @param idx Index for the monster in the battle.
 */
inline Monster *get_monster(uint8_t idx) {
  return encounter.monsters + idx;
}

/**
 * Resets player combat stats and flags at the start of each round.
 */
void reset_player_stats(void) NONBANKED;

/**
 * Resets a monster's combat stats and flags at the start of each round.
 * @param m Monster instance to reset.
 */
void monster_reset_stats(Monster *m) NONBANKED;

/**
 * Applies a status effect and adds it to the given list.
 * @param list Effects lists.
 * @param effect Status effect to apply.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
StatusEffectResult apply_status_effect(
  StatusEffectInstance *list,
  StatusEffect effect,
  uint8_t flag,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) BANKED;

/**
 * Handles the player "flee" action.
 * @return Whether or not the player could flee.
 */
void player_flee(void);

/**
 * Applies rewards after a successful encounter. Handles XP, leveling, etc.
 */
void apply_rewards(void);

/**
 * @return Whether or not a slot is available for the given status effect.
 */
bool has_effect_slot(
  StatusEffectInstance *list,
  StatusEffect effect,
  PowerTier tier,
  uint8_t duration
);

/**
 * Attempts to apply the 'blind' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_blind(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_BLIND;
  const DebuffFlag flag = FLAG_DEBUFF_BLIND;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'scared' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_scared(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_SCARED;
  const DebuffFlag flag = FLAG_DEBUFF_SCARED;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'paralyzed' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_paralyzed(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_PARALYZED;
  const DebuffFlag flag = FLAG_DEBUFF_PARALYZED;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'poisoned' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_poison(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_POISONED;
  const DebuffFlag flag = FLAG_DEBUFF_POISONED;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'confused' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_confused(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_CONFUSED;
  const DebuffFlag flag = FLAG_DEBUFF_CONFUSED;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'AGL down' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_agl_down(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_AGL_DOWN;
  const DebuffFlag flag = FLAG_DEBUFF_AGL_DOWN;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'ATK down' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_atk_down(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_ATK_DOWN;
  const DebuffFlag flag = FLAG_DEBUFF_ATK_DOWN;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'DEF down' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @param immune Debuff immunities for the entity.
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_def_down(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration,
  uint8_t immune
) {
  const StatusEffect effect = DEBUFF_DEF_DOWN;
  const DebuffFlag flag = FLAG_DEBUFF_DEF_DOWN;
  return apply_status_effect(list, effect, flag, tier, duration, immune);
}

/**
 * Attempts to apply the 'haste' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_haste(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration
) {
  const StatusEffect effect = BUFF_HASTE;
  const DebuffFlag flag = FLAG_BUFF_HASTE;
  return apply_status_effect(list, effect, flag, tier, duration, 0);
}

/**
 * Attempts to apply the 'regen' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_regen(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration
) {
  const StatusEffect effect = BUFF_REGEN;
  const DebuffFlag flag = FLAG_BUFF_REGEN;
  return apply_status_effect(list, effect, flag, tier, duration, 0);
}

/**
 * Attempts to apply the 'AGL up' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_agl_up(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration
) {
  const StatusEffect effect = BUFF_AGL_UP;
  const DebuffFlag flag = FLAG_BUFF_AGL_UP;
  return apply_status_effect(list, effect, flag, tier, duration, 0);
}

/**
 * Attempts to apply the 'ATK up' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_atk_up(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration
) {
  const StatusEffect effect = BUFF_ATK_UP;
  const DebuffFlag flag = FLAG_BUFF_ATK_UP;
  return apply_status_effect(list, effect, flag, tier, duration, 0);
}

/**
 * Attempts to apply the 'DEF up' status effect.
 * @param list List of status effects for the entity.
 * @param tier Power tier for the effect (potency, basically).
 * @param duration Duration for the effect (0 means endless).
 * @return The result of the status effect application.
 */
inline StatusEffectResult apply_def_up(
  StatusEffectInstance *list,
  PowerTier tier,
  uint8_t duration
) {
  const StatusEffect effect = BUFF_DEF_UP;
  const DebuffFlag flag = FLAG_BUFF_DEF_UP;
  return apply_status_effect(list, effect, flag, tier, duration, 0);
}

#endif
