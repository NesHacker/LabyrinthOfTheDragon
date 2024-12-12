#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>
#include "tables.h"

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
  TARGET_SELF,
  TARGET_SINGLE,
  TARGET_ALL,
} TargetType;

/**
 * Defines an ability that can be used by the player or a monster.
 */
typedef struct Ability {
  uint8_t level;
  char name[12];
  uint8_t sp_cost;
  TargetType target_type;
} Ability;

/**
 * Defines a summon that the player can possess and fight as.
 */
typedef struct Summon {
  uint8_t id;
  char name[12];

  uint8_t agl;

  uint8_t vuln;
  uint8_t resist;
  uint8_t immune;

  Ability druid_ability[5];
  Ability fighter_ability[5];
  Ability monk_ability[5];
  Ability sorcerer_ability[5];
} Summon;

/**
 * Data representation of the player.
 */
typedef struct Player {
  PlayerClass player_class;
  uint8_t level;
  uint16_t xp;
  uint16_t next_level_xp;

  uint8_t hp;
  uint8_t max_hp;
  uint8_t mp;
  uint8_t max_mp;

  uint8_t atk;
  uint8_t def;
  uint8_t matk;
  uint8_t mdef;

  Summon *current_summon;
  Summon *default_summon;
} Player;

#endif
