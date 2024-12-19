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
   * Maximum HP (health points).
   */
  uint16_t max_hp;
  /**
   * Current HP (health points).
   */
  uint16_t hp;
  /**
   * Target HP after taking damage (used for animations).
   */
  uint16_t target_hp;
  /**
   * Delta to use when animating HP changes.
   */
  int16_t hp_delta;
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
   * Active status effects. Monsters can only have four effects active at once.
   */
  StatusEffectInstance status_effects[MAX_ACTIVE_EFFECTS];
  /**
   * Whether or not the monster can flee (set this for elites, bosses, etc.).
   */
  bool can_flee;
  /**
   * Set to `true` if the monster has successfully fled.
   */
  bool fled;
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
 * Initializes a monster instance for a monster generator.
 * @param i Monster instance to reset.
 * @param m Base monster for the instance.
 */
void monster_init_instance(MonsterInstance *i, Monster *m);

/**
 * Deactivates a monster instance and removes it from combat.
 */
inline void monster_deactivate(MonsterInstance *m) {
  m->active = false;
}

/**
 * Generates a test dummy monster.
 * @param m Monster instance to populate.
 * @param level Level for the dummy.
 * @param invincible Whether or not the dummy is invincible.
 */
void dummy_generator(MonsterInstance *m, uint8_t level, bool invincible);

/**
 * Basic kobold generator.
 * @param level Level for the kobold to generate.
 */
void kobold_generator(MonsterInstance *m, uint8_t level, PowerTier tier);


#endif
