#ifndef _MONSTER_H
#define _MONSTER_H

#include <stdint.h>
#include <stdbool.h>
#include <gb/gb.h>
#include <gb/cgb.h>

#include "core.h"
#include "stats.h"

/**
 * Number of tiles in a full monster tileset.
 */
#define MONSTER_TILES 7 * 7 * 2

/**
 * Defines a monster and its behaviors.
 */
typedef struct Monster {
  /**
   * Unique identifier for the monster.
   */
  const uint8_t id;
  /**
   * Nine character display name for the monster.
   */
  const char name[9];
  /**
   * Tileset for the monster.
   */
  const Tileset *tileset;
} Monster;

/**
 * An instance of a monster that is used for battle.
 */
typedef struct MonsterInstance {
  /**
   * Whether or not the monster is active in combat and can take a turn.
   */
  bool active;
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
  uint16_t hp;
  /**
   * Maximum HP (health points).
   */
  uint16_t max_hp;
  /**
   * Target HP after taking damage (used for animations).
   */
  uint16_t target_hp;
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
  /**
   * Parameter used when generating this monster instance. Can be used to alter
   * behaviors in the `take_turn` handler.
   */
  uint8_t parameter;
  /**
   * Takes a turn for the given monster instance.
   * @param m Monster instance for which to take a turn.
   */
  void (*take_turn)(struct MonsterInstance *m);
} MonsterInstance;

/**
 * Monster 1 - The Kobold.
 */
extern const Monster MONSTER_KOBOLD;

/**
 * Basic kobold generator.
 * @param level Level for the kobold to generate.
 */
void kobold_generator(MonsterInstance *m, uint8_t level, PowerTier tier);

/**
 * Monster 2 - The Beholder.
 */
extern const Monster MONSTER_BEHOLDER;

/**
 * Initializes a monster instance for a monster generator.
 * @param i Monster instance to reset.
 * @param m Base monster for the instance.
 */
void monster_init_instance(MonsterInstance *i, Monster *m);

/**
 * Resets base stats at the start of each round of combat.
 * @param m Monster instance to reset.
 */
void monster_reset_stats(MonsterInstance *m);

/**
 * Deactivates a monster instance and removes it from combat.
 */
inline void monster_deactivate(MonsterInstance *m) {
  m->active = false;
}

#endif
