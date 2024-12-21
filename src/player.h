#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

#include "monster.h"
#include "stats.h"
#include "tables.h"
#include "text_writer.h"

/**
 * Maximum number of abilities a player can acquire.
 */
#define MAX_ABILITIES 6

/**
 * Level to set for new characters.
 */
#define NEW_CHARACTER_LEVEL 5

/**
 * Enumeration of all classes in the game. Decides stat tiers, abilities, etc.
 */
typedef enum PlayerClass {
  CLASS_DRUID,
  CLASS_FIGHTER,
  CLASS_MONK,
  CLASS_SORCERER,
  CLASS_TEST = 0xFF,
} PlayerClass;

/**
 * Flags that determine if a player has a given ability.
 */
typedef enum AbilityFlag {
  ABILITY_0 = FLAG(0),
  ABILITY_1 = FLAG(1),
  ABILITY_2 = FLAG(2),
  ABILITY_3 = FLAG(3),
  ABILITY_4 = FLAG(4),
  ABILITY_5 = FLAG(5),
  ABILITY_ALL = 0b00111111,
} AbilityFlag;

/**
 * Enumeration of all targeting types for abilities.
 */
typedef enum TargetType {
  /**
   * Indicates an ability targets the player.
   */
  TARGET_SELF,
  /**
   * Indicates an ability that targets a single enemy.
   */
  TARGET_SINGLE,
  /**
   * Indicates an ability targets all enemies.
   */
  TARGET_ALL,
} TargetType;

/**
 * Defines an ability that can be used by the player or a monster.
 */
typedef struct Ability {
  /**
   * Non zero id for the ability.
   */
  uint8_t id;
  /**
   * Ability's in game name.
   */
  const char *name;
  /**
   * Taegeting mode for the ability.
   */
  TargetType target_type;
  /**
   * SP cost required to activate and consumed by the ability.
   */
  uint8_t sp_cost;
  /**
   * Callback to execute the ability in battle mode.
   */
  void (*execute)(void);
} Ability;

/**
 * Data representation of the player.
 */
typedef struct Player {
  /**
   * Player's name.
   */
  char name[8];
  /**
   * The player's class.
   */
  PlayerClass player_class;
  /**
   * Flags that denote if the player has access to a special ability. These are
   * defined for each class.
   */
  uint8_t ability_flags;
  /**
   * Current experience level.
   */
  uint8_t level;
  /**
   * Current experience points.
   */
  uint16_t exp;
  /**
   * How many points are required for the player to reach the next level.
   */
  uint16_t next_level_exp;
  /**
   * Auto page message speed for battle messages.
   */
  AutoPageSpeed message_speed;
  /**
   * Current HP (health points).
   */
  uint16_t hp;
  /**
   * Maximum HP (health points).
   */
  uint16_t max_hp;
  /**
   * Current SP (skill points) for martial classes and MP (magic points) for
   * magic classes.
   */
  uint16_t sp;
  /**
   * Maximum SP (skill points) for martial classes and MP (magic points) for
   * magic classes.
   */
  uint16_t max_sp;
  /**
   * Base attack score.
   */
  uint8_t atk_base;
  /**
   * Current attack score. Calculated via status effects, etc.
   */
  uint8_t atk;
  /**
   * Base defense score.
   */
  uint8_t def_base;
  /**
   * Current defense. Calculated via status effects, etc.
   */
  uint8_t def;
  /**
   * Base magical attack score.
   */
  uint8_t matk_base;
  /**
   * Current magical attack. Calculated via status effects, etc.
   */
  uint8_t matk;
  /**
   * Base magical defense score.
   */
  uint8_t mdef_base;
  /**
   * Current magical defense. Calculated via status effects, etc.
   */
  uint8_t mdef;
  /**
   * Base agility.
   */
  uint8_t agl_base;
  /**
   * Current agility. Via status effects, etc.
   */
  uint8_t agl;
  /**
   * Aspected damage immunity bitfield. Each bit corresponds to a different
   * immunity.
   * @see `DamageAspect`
   */
  uint8_t aspect_immune;
  /**
   * Aspected damage resistances bitfield. Eeach bit corresponds to a different
   * resistance.
   * @see `DamageAspect`
   */
  uint8_t aspect_resist;
  /**
   * Aspected damage vulnerability bitfield. Each bit represents a different
   * vulnerability.
   * @see `DamageAspect`
   */
  uint8_t aspect_vuln;
  /**
   * Debuff immunity. Each bit corresponds to a different immunity.
   * @see `StatusEffectImmunity`
   */
  uint8_t debuff_immune;
  /**
   * Active debuff bitfield.
   */
  uint8_t debuffs;
  /**
   * Active buff bitfield.
   */
  uint8_t buffs;
  /**
   * Whether or not the player has found the torch.
   */
  bool has_torch;
  /**
   * Gauge that represents how long the torch can stay lit.
   */
  uint8_t torch_gauge;
  /**
   * Number of magic keys the player currently posesses.
   */
  uint8_t magic_keys;
} Player;

/**
 * The global player object. Used by various systems (e.g. world map, battle,
 * etc.) to handle player related logic and graphics.
 */
extern Player player;

/**
 * All abilities for the current player class.
 */
extern const Ability *class_abilities[6];

/**
 * Abilities the player currently has access to.
 */
extern const Ability *player_abilities[6];

/**
 * Number of abilities the player has access to.
 */
extern uint8_t player_num_abilities;

/**
 * Used to denote an empty or null ability in ability lists.
 */
extern const Ability null_ability;

/**
 * Initializes a new player with the given class.
 */
void init_player(PlayerClass player_class);

/**
 * Initializes the player using the test class.
 * @param level Level for the test player.
 */
void init_test_player(uint8_t level);

/**
 * Grants the player a new ability.
 * @param flag The flag for the ability.
 */
void grant_ability(AbilityFlag flag);

/**
 * Sets class abilities based on the current player class.
 */
void set_class_abilities(void);

/**
 * Sets player abilities based on current ability flags.
 */
void set_player_abilities(void);

/**
 * Performs a basic attack against the the targeted monster.
 */
void player_base_attack(void);

/**
 * Full heals the player, setting HP and SP to their max values.
 */
void full_heal_player(void);

/**
 * Sets the player's level to a specific value.
 */
void set_player_level(uint8_t level);

/**
 * @return `true` if the player has a magic combat class.
 */
inline bool is_magic_class(void) {
  return player.player_class == CLASS_DRUID ||
    player.player_class == CLASS_SORCERER;
}

/**
 * @return `true` if the player has a martial combat class.
 */
inline bool is_martial_class(void) {
  return !is_magic_class();
}

/**
 * @return `true` if the player has leveled up.
 */
inline bool has_leveled(void) {
  return player.next_level_exp >= player.exp;
}

#endif
