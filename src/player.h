#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

#include "stats.h"
#include "tables.h"

/*
  TODO Implement player saves

  Should be pretty easy,

  uint8_t *cart_ram = (void *)(0xA000 + save_offset);
  // calculate checksum over all player data
  // store checksum at beginning then memcopy the entire player model
*/

/**
 * Enumeration of all classes in the game. This is primarily used to determine
 * the overworld graphics and the summon abilities available to a player.
 */
typedef enum PlayerClass {
  CLASS_DRUID,
  CLASS_FIGHTER,
  CLASS_MONK,
  CLASS_SORCERER,
} PlayerClass;

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
   * Ability's in game name.
   */
  const char name[12];
  /**
   * Player class to which the ability belongs.
   */
  PlayerClass player_class;
  /**
   * Cosmetic power tier for the ability.
   */
  PowerTier tier;
  /**
   * Unlock level for the ability.
   */
  uint8_t level;
  /**
   * Taegeting mode for the ability.
   */
  TargetType target_type;
  /**
   * SP cost required to activate and consumed by the ability.
   */
  uint8_t sp_cost;
  /**
   * SP cost display tiles.
   */
  const uint8_t sp_cost_tiles[3];
  /**
   * Callback to execute the ability in battle mode.
   */
  void (*execute)(void);
  /**
   * Pointer to the next ability in an ability list.
   */
  struct Ability *next;
} Ability;

/**
 * Defines a summon that the player can possess and fight as.
 */
typedef struct Summon {
  /**
   * Unique id for the summon.
   */
  uint8_t id;
  /**
   * Name of the summon.
   */
  char name[12];
  /**
   * Boss / summon character name.
   */
  char npc_name[8];
  /**
   * Agility power tier.
   */
  PowerTier agl_tier;
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
   * Initialization function called when the player set the summon as active.
   */
  void (*activate)(void);
  /**
   * Base attack method to be executed when selected during battle.
   */
  void (*base_attack)(void);
  /**
   * Pointer to a linked list of abilities for the summon. For the most part
   * this list is static but is initialized once a player's class has been
   * determined (new game, loading save, etc.).
   *
   * Keeping this as a linked list to allow for ability growth without having
   * to redefine a table of class associated summon abilities (read: it's
   * easier for me to code xD).
   */
  Ability *ability;
} Summon;

/**
 * Data representation of the player.
 */
typedef struct Player {
  /**
   * The player's class.
   */
  PlayerClass player_class;
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
   * Current HP (health points).
   */
  uint8_t hp;
  /**
   * Maximum HP (health points).
   */
  uint8_t max_hp;
  /**
   * Current SP (skill points) for martial classes and MP (magic points) for
   * magic classes.
   */
  uint8_t sp;
  /**
   * Maximum SP (skill points) for martial classes and MP (magic points) for
   * magic classes.
   */
  uint8_t max_sp;
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
   * Summon currently active in battle.
   */
  Summon *summon;
  /**
   * Summon to activate by default prior to battle.
   */
  Summon *default_summon;
} Player;


// Externs, prototypes, inlines, etc.

/**
 * The global player object. Used by various systems (e.g. world map, battle,
 * etc.) to handle player related logic and graphics.
 */
extern Player player;

// TODO Temporary, remove when adding save system, etc.
void init_player(void);

/**
 * Wilbur the Kobold. Defines all properties and methods for the
 * summon.
 */
extern Summon summon_kobold;

/**
 * "Vine Whip" kobold druid ability.
 */
extern Ability kobold_vine_whip;

/**
 * "Mend" kobold druid ability.
 */
extern Ability kobold_mend;

/**
 * "Summon Ants" kobold druid ability.
 */
extern Ability kobold_summon_ants;

/**
 * "Dirty Fang" kobold druid ability.
 */
extern Ability kobold_dirty_fang;

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

#endif
