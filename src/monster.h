#ifndef _MONSTER_H
#define _MONSTER_H

#include <stdint.h>
#include <stdbool.h>
#include <gb/gb.h>
#include <gb/cgb.h>

#include "stats.h"

/**
 * Defines a monster and its behaviors.
 */
typedef struct Monster {
  /**
   * Unique identifier for the monster.
   */
  uint8_t id;
  /**
   * Nine character display name for the monster.
   */
  char name[9];
  /**
   * ROM bank where the monster's tile data resides.
   */
  uint8_t tile_bank;
  /**
   * Pointer to the tile data for the monster.
   */
  uint8_t *tile_data;
  /**
   * Generates a new instance of the monster.
   * @param m Instance to initialize.
   * @param level Level for the instance.
   * @param tier Power tier for the instance.
   */
  void (*new_instance)(uint8_t pos, uint8_t level, PowerTier tier);
  /**
   * Takes a turn for the given monster instance.
   * @param m Monster instance for which to take a turn.
   */
  void (*take_turn)(uint8_t pos);
} Monster;

/**
 * An instance of a monster that is used for battle.
 */
typedef struct MonsterInstance {
  /**
   * Monster definition that generated this instance.
   */
  const Monster *monster;
  /**
   * Palette colors for the instance.
   */
  const palette_color_t *palette;
  /**
   * Single character used to reference the instance in battle if there are
   * more than one instance of a single monster.
   */
  char id;
  /**
   * Monster level.
   */
  uint8_t level;
  /**
   * The power tier to use when awarding experience for the monster.
   */
  PowerTier exp_tier;
  /**
   * Current HP (health points).
   */
  uint8_t hp;
  /**
   * Maximum HP (health points).
   */
  uint8_t max_hp;
  /**
   * Base physical attack score.
   */
  uint8_t atk_base;
  /**
   * Current physical attack. Calculated via status effects, etc.
   */
  uint8_t atk;
  /**
   * Base physical defense score.
   */
  uint8_t def_base;
  /**
   * Current physical defense. Calculated via status effects, etc.
   */
  uint8_t def;
  /**
   * Base magical attack score.
   */
  uint8_t matk_base;
  /**
   * Current magical attack. Calculed via status effects, etc.
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
   * Base agility score.
   */
  uint8_t agl_base;
  /**
   * Current agility. Calculated via status effects, etc.
   */
  uint8_t agl;
  /**
   * Aspected damage immunity bitfield.
   * @see `DamageAspect`
   */
  uint8_t aspect_immune;
  /**
   * Aspected damage resistances bitfield.
   * @see `DamageAspect`
   */
  uint8_t aspect_resist;
  /**
   * Aspected damage vulnerability bitfield.
   * @see `DamageAspect`
   */
  uint8_t aspect_vuln;
  /**
   * Debuff immunity.
   * @see `StatusEffectImmunity`
   */
  uint8_t debuff_immune;
} MonsterInstance;

/**
 * Monster 1 - The Kobold.
 */
extern const Monster MONSTER_KOBOLD;

/**
 * Monster 2 - The Beholder.
 */
extern const Monster MONSTER_BEHOLDER;

/**
 * Fully resets / clears the fields of the given monster instance.
 * @param m Monster instance to reset.
 */
void reset_monster_instance(MonsterInstance *m);

#endif
