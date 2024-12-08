#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

/**
 * Enumeration of all classes in the game. This is primarily used to determine
 * the overworld graphics and the summon abilities available to a player.
 */
typedef enum PlayerClass {
  CLASS_MONSTER,
  CLASS_DRUID,
  CLASS_FIGHTER,
  CLASS_MONK,
  CLASS_SORCERER,
} PlayerClass;

/**
 * Enumeration of all ability types.
 */
typedef enum AbilityType {
  ABILITY_FIGHT,
  ABILITY_TYPE_MAGIC,
  ABILITY_TYPE_TECH,
} AbilityType;

/**
 * Defines an ability that can be used by the player or a monster.
 */
typedef struct Ability {
  uint8_t id;
  AbilityType type;
  PlayerClass player_class;
  char name[12];
  uint8_t min_level;
} Ability;

/**
 * Defines a summon that the player can possess and fight as.
 */
typedef struct Summon {
  uint8_t id;
  char name[12];
} Summon;

/**
 * Data representation of the player.
 */
typedef struct Player {
  uint8_t checksum;

  uint8_t id;
  char name[6];
  PlayerClass player_class;

  uint8_t summon_id;

  uint8_t level;

  uint8_t hp;
  uint8_t max_hp;
  uint8_t mp;
  uint8_t max_mp;

  uint8_t attack;
  uint8_t armor_class;
  uint8_t magic_attack;
  uint8_t saving_throw;
  uint8_t initiative;

  // hours & minutes played, etc.
} Player;

/**
 * Universal null/empty ability.
 */
extern const Ability NULL_ABILITY;

#endif
