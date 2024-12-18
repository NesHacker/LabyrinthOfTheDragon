/**
 * Macros and routines for calculating various battle stats and rolls.
 */
#ifndef _STATS_H
#define _STATS_H

#include <rand.h>
#include <stdint.h>

#include "core.h"
#include "tables.h"
#include "strings.h"

/**
 * Enumeration of the four primary power tiers used when calculating game
 * statistics.
 */
typedef enum PowerTier {
  C_TIER,
  B_TIER,
  A_TIER,
  S_TIER,
} PowerTier;

/**
 * Core damage types for the game. Aspects color damage allowing for the player
 * and monsters to express vulnerabilities, resistances, and immunities.
 */
typedef enum DamageAspect {
  DAMAGE_PHYSICAL = FLAG(0),
  DAMAGE_MAGICAL = FLAG(1),
  DAMAGE_EARTH = FLAG(2),
  DAMAGE_WATER = FLAG(3),
  DAMAGE_AIR = FLAG(4),
  DAMAGE_FIRE = FLAG(5),
  DAMAGE_LIGHT = FLAG(6),
  DAMAGE_DARK = FLAG(7),
} DamageAspect;

/**
 * @return The string name for the given aspect type.
 * @param type Aspect type to convert.
 */
inline const char *damage_aspect_name(DamageAspect type) {
  switch (type) {
  case DAMAGE_PHYSICAL:
    return str_misc_physical;
  case DAMAGE_MAGICAL:
    return str_misc_magical;
  case DAMAGE_EARTH:
    return str_misc_earth;
  case DAMAGE_WATER:
    return str_misc_water;
  case DAMAGE_AIR:
    return str_misc_air;
  case DAMAGE_FIRE:
    return str_misc_fire;
  case DAMAGE_LIGHT:
    return str_misc_light;
  default:
    return str_misc_dark;
  }
}

/**
 * Number of status effects supported by the game.
 */
#define STATUS_EFFECTS 16

/**
 * Enumeration of all status effects (buffs & debuffs) in the game.
 */
typedef enum StatusEffect {
  DEBUFF_BLIND,
  DEBUFF_SCARED,
  DEBUFF_PARALZYED,
  DEBUFF_POISONED,
  DEBUFF_CONFUSED,
  DEBUFF_AGL_DOWN,
  DEBUFF_ATK_DOWN,
  DEBUFF_DEF_DOWN,
  BUFF_UNUSED_0,
  BUFF_UNUSED_1,
  BUFF_UNUSED_2,
  BUFF_HASTE,
  BUFF_REGEN,
  BUFF_AGL_UP,
  BUFF_ATK_UP,
  BUFF_DEF_UP,
} StatusEffect;

/**
 * Denotes a status effect immunity for the player or a monster.
 */
typedef enum StatusEffectImmunity {
  IMMUNE_BLIND = FLAG(0),
  IMMUNE_SCARED = FLAG(1),
  IMMUNE_PARALYZED = FLAG(2),
  IMMUNE_POISON = FLAG(3),
  IMMUNE_CONFUSED = FLAG(4),
  IMMUNE_AGL_DOWN = FLAG(5),
  IMMUNE_ATK_DOWN = FLAG(6),
  IMMUNE_DEF_DOWN = FLAG(7),
} StatusEffectImmunity;

/**
 * Status effect instance entry. These are used to track status effects during
 * encounters.
 */
typedef struct StatusEffectInstance {
  bool active;
  uint8_t duration;
  PowerTier tier;
} StatusEffectInstance;

/**
 * Used to determine hit chance based on the difference between the attacker's
 * ATK and the defender's DEF. Yields a maximum chance to hit of 95% and a
 * minimum of 20%.
 *
 * If (ATK - DEF) < 0 then element [0] is always used, if (ATK - DEF) > 32 then
 * element [64] is always used. Otherwise use element [ATK - DEF + 32] to get
 * the "target" number a `rand()` result must fall below for a successful
 * attack.
 */
extern uint8_t attack_roll_target[65];

/**
 * Used to add variance to damage rolls. The entries in this array are numerator
 * multipliers for the equation: `mod[rand(0, 16)] * BASE_DMG / 16`.
 */
extern const uint8_t damage_roll_modifier[16];

/**
 * Level relative XP modifier table.
 *
 * Used to modify XP rewards for monsters that are lower or higher level
 * relative to the player. Monsters more than 8 levels below a player award no
 * XP bonus. Similarly monster 7 levels or greater above a the player award a
 * maximum of 200% base XP.
 *
 * Index 0 corresponds to the monster being eight levels below the player, and
 * index 15 to the monster being 7 levels above the player.
 *
 * The value given by the table is used as a numerator in the following
 * equation: `xp_award = xp_mod * base_xp / 16`. Where the `base_xp` is
 * calculated using the monster's level and tier.
 */
extern const uint8_t xp_mod[16];


/**
 * @return Experience points required to achieve the given level.
 * @param level The level.
 */
uint16_t get_exp(uint8_t level) BANKED;

/**
 * @return Experience points awarded for the monster.
 * @param level The level of the monster.
 * @param tier Power tier for the monster.
 */
uint16_t get_monster_exp(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Agility for a summon or monster.
 * @param level Level of the monster/summon.
 * @param tier Power tier for the stat.
 */
uint8_t get_agl(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Maximum hp for the player.
 * @param level level of the player.
 * @param tier power tier for the stat.
 */
uint16_t get_player_hp(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Maximum sp for the player.
 * @param level level of the player.
 * @param tier power tier for the stat.
 */
uint8_t get_player_sp(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Defense for the player.
 * @param level level of the player.
 * @param tier power tier for the stat.
 */
uint8_t get_player_def(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Attack for the player.
 * @param level level of the player.
 * @param tier power tier for the stat.
 */
uint8_t get_player_atk(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Damage for the player.
 * @param level level of the player.
 * @param tier power tier for the stat.
 */
uint16_t get_player_damage(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Maximum HP for the monster.
 * @param level Level of the monster.
 * @param tier Power tier for the stat.
 */
uint16_t get_monster_hp(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Defense for the monster.
 * @param level Level of the monster.
 * @param tier Power tier for the stat.
 */
uint8_t get_monster_def(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Attack for the monster.
 * @param level Level of the monster.
 * @param tier Power tier for the stat.
 */
uint8_t get_monster_atk(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Damage for the monster.
 * @param level Level of the monster.
 * @param tier Power tier for the stat.
 */
uint16_t get_monster_dmg(uint8_t level, PowerTier tier) BANKED;

/**
 * @return Healing for a player healing ability.
 * @param level Level of the player.
 * @param tier Power tier for the ability.
 */
uint16_t get_player_heal(uint8_t level, PowerTier tier) BANKED;

/**
 * Performs an attack roll.
 * @param atk Attacker's ATK score.
 * @param def Defender's DEF score.
 * @return `true` if the attack succeeds.
 */
bool roll_attack(uint8_t atk, uint8_t def) BANKED;

/**
 * @return An attack's resulting damage given a roll.
 * @param d16_roll The d16 roll.
 * @param base_dmg The base damage for the attack.
 */
uint16_t calc_damage(uint8_t d16_roll, uint16_t base_dmg) BANKED;

/**
 * @return Experience awarded to a player of the given level from a monster.
 * @param level Level of the monster.
 * @param tier Power tier for the monster.
 */
uint16_t calc_monster_exp(uint8_t level, PowerTier tier) BANKED;

/**
 * @return A random d2 roll.
 */
inline uint8_t d2(void) {
  return rand() & 0x01;
}

/**
 * @return A random d4 roll.
 */
inline uint8_t d4(void) {
  return rand() & 0x03;
}

/**
 * @return A random d8 roll.
 */
inline uint8_t d8(void) {
  return rand() & 0x07;
}

/**
 * @return A random d16 roll.
 */
inline uint8_t d16(void) {
  return rand() & 0x0F;
}

/**
 * @return A random d16 roll.
 */
inline uint8_t d32(void) {
  return rand() & 0x1F;
}

/**
 * @return A random d64 roll.
 */
inline uint8_t d64(void) {
  return rand() & 0x3F;
}

/**
 * @return A random d128 roll.
 */
inline uint8_t d128(void) {
  return rand() & 0x7F;
}

/**
 * @return A random d256 roll.
 */
inline uint8_t d256(void) {
  return rand();
}

/**
 * Calculates a level offset while keeping the level in bounds from 1 to 99.
 * @param level The level to offset.
 * @param offset The amount by which to offset the level.
 * @return The level with the offset applied capped to the range 1 to 99.
 */
inline uint8_t level_offset(uint8_t level, int8_t offset) {
  if ((int8_t)level + offset < 1)
    return 1;
  if ((int8_t)level + offset > 99)
    return 99;
  return level + offset;
}

/**
 * @return `true` if the damage / healing roll is critical.
 * @param d16_roll Result of a d16 roll.
 */
inline bool is_critical(uint8_t d16_roll) {
  return d16_roll >= 14;
}

/**
 * @return `true` if the damage / healing roll is a fumble.
 * @param d16_roll Result of a d16 roll.
 */
inline bool is_fumble(uint8_t d16_roll) {
  return d16_roll <= 1;
}

#endif