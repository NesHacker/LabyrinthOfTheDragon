#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

#include "map.h"

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
  CLASS_DRUID = 0 ,
  CLASS_FIGHTER = 1,
  CLASS_MONK = 2,
  CLASS_SORCERER = 3,
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
 * Special flags that can be set during battle by abilities.
 */
typedef enum SpecialFlags {
  SPECIAL_BARKSKIN = FLAG(0),
  SPECIAL_HASTE = FLAG(1),
  SPECIAL_EVASION = FLAG(2),
  SPECIAL_SLEET_STORM = FLAG(3),
  SPECIAL_INSTANT_KILL = FLAG(7),
} SpecialFlags;

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
   * The color of the flame with which the torch is lit.
   */
  FlameColor torch_color;
  /**
   * Number of magic keys the player currently posesses.
   */
  uint8_t magic_keys;
  /**
   * Whether or not the player has collected their first magic key (for UI).
   */
  bool got_magic_key;
  /**
   * Special battle flags that the player can have.
   */
  uint8_t special_flags;
  /**
   * Number of turns the player is tripped and prone.
   */
  uint8_t trip_turns;
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
 * Grants the player a new ability.
 * @param flag The flag for the ability.
 */
void grant_ability(AbilityFlag flag) BANKED;

/**
 * @param flag Ability flag just granted.
 * @return THe message to display upon granting the ability.
 */
const char *get_grant_message(AbilityFlag flag) BANKED;

/**
 * Sets the player's level to a specific value.
 * @param level Level to set.
 */
void set_player_level(uint8_t level) BANKED;

/**
 * Initializes a new player with the given class.
 */
void init_player(PlayerClass player_class) BANKED;

/**
 * Adds the given experience points and performs a level up if applicable.
 * @param xp Experience points to add.
 * @return `true` if the player leveled up.
 */
bool level_up(uint16_t xp) BANKED;

/**
 * Performs a basic attack against the the targeted monster.
 */
void player_base_attack(void) BANKED;

/**
 * Executes a player ability.
 * @param ability Ability to execute.
 */
void player_use_ability(const Ability *ability) BANKED;

/**
 * Full heals the player, setting HP and SP to their max values.
 */
inline void full_heal_player(void) {
  player.hp = player.max_hp;
  player.sp = player.max_sp;
  player_hp_and_sp_updated = true;
}

/**
 * Sets the player's HP and SP.
 */
inline void set_hp_and_sp(uint16_t hp, uint16_t sp) {
  if (hp > player.max_hp)
    player.hp = player.max_hp;
  else
    player.hp = hp;

  if (sp > player.max_sp)
    player.sp = player.max_sp;
  else
    player.sp = sp;

  player_hp_and_sp_updated = true;
}

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

/**
 * Clears all special flags for the player.
 */
inline void reset_special(void) {
  player.special_flags = 0;
}

/**
 * Applies a special flag to the player.
 * @param s Flag to set.
 */
inline void apply_special(SpecialFlags s) {
  player.special_flags |= s;
}

/**
 * Removes a special flag from the character.
 * @param s Flag to remove.
 */
inline void remove_special(SpecialFlags s) {
  player.special_flags &= ~s;
}

/**
 * @return `true` if the given special flag is set for the player.
 * @param s Flag to test.
 */
inline bool has_special(SpecialFlags s) {
  return player.special_flags & s;
}

/**
 * Palettes for each of the hero classes.
 */
extern const palette_color_t hero_colors[];

// Ability prototypes and externs

void druid_base_attack(void);
void druid_cure_wounds(void);
void druid_bark_skin(void);
void druid_lightning(void);
void druid_heal(void);
void druid_insect_plague(void);
void druid_regen(void);

extern const Ability druid0;
extern const Ability druid1;
extern const Ability druid2;
extern const Ability druid3;
extern const Ability druid4;
extern const Ability druid5;

void fighter_base_attack(void);
void fighter_second_wind(void);
void fighter_action_surge(void);
void fighter_cleave(void);
void fighter_trip_attack(void);
void fighter_menace(void);
void fighter_indomitable(void);

extern const Ability fighter0;
extern const Ability fighter1;
extern const Ability fighter2;
extern const Ability fighter3;
extern const Ability fighter4;
extern const Ability fighter5;

void monk_base_attack(void);
void monk_evasion(void);
void monk_open_palm(void);
void monk_still_mind(void);
void monk_flurry(void);
void monk_diamond_body(void);
void monk_quivering_palm(void);

extern const Ability monk0;
extern const Ability monk1;
extern const Ability monk2;
extern const Ability monk3;
extern const Ability monk4;
extern const Ability monk5;

void sorcerer_base_attack(void);
void sorcerer_darkness(void);
void sorcerer_fireball(void);
void sorcerer_haste(void);
void sorcerer_sleetstorm(void);
void sorcerer_disintegrate(void);
void sorcerer_wild_magic(void);

extern const Ability sorcerer0;
extern const Ability sorcerer1;
extern const Ability sorcerer2;
extern const Ability sorcerer3;
extern const Ability sorcerer4;
extern const Ability sorcerer5;

void test_class_base_attack(void);
void test_class_ability0(void);
void test_class_ability1(void);
void test_class_ability2(void);
void test_class_ability3(void);
void test_class_ability4(void);
void test_class_ability5(void);

extern const Ability test_class0;
extern const Ability test_class1;
extern const Ability test_class2;
extern const Ability test_class3;
extern const Ability test_class4;
extern const Ability test_class5;

#endif
