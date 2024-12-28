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
 * Monster ids for all the monsters in the game.
 */
typedef enum MonsterType {
  MONSTER_KOBOLD,
  MONSTER_GOBLIN,
  MONSTER_ZOMBIE,
  MONSTER_BUGBEAR,
  MONSTER_OWLBEAR,
  MONSTER_GELATINOUS_CUBE,
  MONSTER_DISPLACER_BEAST,
  MONSTER_WILL_O_WISP,
  MONSTER_DEATHKNIGHT,
  MONSTER_MINDFLAYER,
  MONSTER_BEHOLDER,
  MONSTER_DRAGON,
  MONSTER_DUMMY = 0xFF,
} MonsterType;

/**
 * An instance of a monster that is used for battle.
 */
typedef struct Monster {
  /**
   * The id for the type of monster.
   */
  MonsterType type;
  /**
   * The tileset used by the instance.
   */
  const Tileset *tileset;
  /**
   * Name for the monster.
   */
  const char *name;
  /**
   * Whether or not the monster is active in combat and can take a turn.
   */
  bool active;
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
   * Active debuff bitfield.
   */
  uint8_t debuffs;
  /**
   * Active buff bitfield.
   */
  uint8_t buffs;
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
   * Routine to determine how the monster acts on its turn.
   */
  void (*take_turn)(struct Monster *m);
} Monster;

/**
 * Deactivates a monster instance and removes it from combat.
 */
inline void monster_deactivate(Monster *monster) {
  monster->active = false;
}

/**
 * General options for Test Dummy monsters.
 */
typedef enum TestDummyType {
  DUMMY_NORMAL,
  DUMMY_INVINCIBLE,
  DUMMY_COWARD,
  DUMMY_AGGRESSIVE,
} TestDummyType;

/**
 * Generates a test dummy monster.
 * @param m Monster instance to populate.
 * @param level Level for the dummy.
 * @param invincible Whether or not the dummy is invincible.
 */
void dummy_generator(
  Monster *m, uint8_t level, TestDummyType type) BANKED;

/**
 * Generates a Kobold.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void kobold_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a Goblin.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void goblin_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a zombie.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void zombie_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a bugbear.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void bugbear_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a owlbear.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void owlbear_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a gelatinous_cube.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void gelatinous_cube_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a displacer beast.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void displacer_beast_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a will-o-wisp.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void will_o_wisp_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a death knight.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void deathknight_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a mind flayer.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void mindflayer_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a beholder.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void beholder_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Generates a dragon.
 * @param m Monster instance to populate.
 * @param level Level for the monster.
 * @param tier Power tier.
 */
void dragon_generator(
  Monster *m, uint8_t level, PowerTier tier) BANKED;

/**
 * Handle the "flee" action for a monster.
 * @param monster Monster who is trying to flee.
 * @return Whether or not the monster could flee.
 */
void monster_flee(Monster *monster) BANKED;

/**
 * Performs a battle turn for the given monster.
 */
void monster_take_turn(Monster *monster) BANKED;

// Data externs (see: monster.data.c, stored on bank 0)

extern const Tileset test_dummy_tileset;
extern const Tileset kobold_tileset;
extern const Tileset goblin_tileset;
extern const Tileset zombie_tileset;
extern const Tileset bugbear_tileset;
extern const Tileset owlbear_tileset;
extern const Tileset gelatinous_cube_tileset;
extern const Tileset displacer_beast_tileset;
extern const Tileset will_o_wisp_tileset;
extern const Tileset deathknight_tileset;
extern const Tileset mindflayer_tileset;
extern const Tileset beholder_tileset;
extern const Tileset dragon_tileset;

extern const palette_color_t dummy_palette[];
extern const palette_color_t kobold_palettes[];
extern const palette_color_t goblin_palettes[];
extern const palette_color_t zombie_palettes[];
extern const palette_color_t bugbear_palettes[];
extern const palette_color_t owlbear_palettes[];
extern const palette_color_t gelatinous_cube_palettes[];
extern const palette_color_t displacer_beast_palettes[];
extern const palette_color_t will_o_wisp_palettes[];
extern const palette_color_t deathknight_palettes[];
extern const palette_color_t mindflayer_palettes[];
extern const palette_color_t beholder_palettes[];
extern const palette_color_t dragon_palettes[];

#endif
