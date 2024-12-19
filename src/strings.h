/*
 * strings.h
 * Extern references for encoded string values across all namespaces.
 */
#ifndef _GAME_STRINGS_H
#define _GAME_STRINGS_H

/**
 * "EMPTY..."
 */
extern const char str_misc_empty[];

/**
 * ""
 */
extern const char str_misc_physical[];

/**
 * "magical"
 */
extern const char str_misc_magical[];

/**
 * "earth"
 */
extern const char str_misc_earth[];

/**
 * "water"
 */
extern const char str_misc_water[];

/**
 * "air"
 */
extern const char str_misc_air[];

/**
 * "fire"
 */
extern const char str_misc_fire[];

/**
 * "light"
 */
extern const char str_misc_light[];

/**
 * "dark"
 */
extern const char str_misc_dark[];

/**
 * "Poison Spray "
 */
extern const char str_ability_druid_poison_spray[];

/**
 * "Cure Wounds  "
 */
extern const char str_ability_druid_cure_wounds[];

/**
 * "Bark Skin    "
 */
extern const char str_ability_druid_bark_skin[];

/**
 * "Lightning    "
 */
extern const char str_ability_druid_ligtning[];

/**
 * "Heal         "
 */
extern const char str_ability_druid_heal[];

/**
 * "Insect Plague"
 */
extern const char str_ability_druid_insect_plague[];

/**
 * "Regenerate   "
 */
extern const char str_ability_druid_regen[];

/**
 * "Poison gas erupts from your palm!"
 */
extern const char str_battle_poison_spray[];

/**
 * "You're enveloped in blue light..."
 */
extern const char str_battle_cure_wounds[];

/**
 * "Your skin grows hard as wood."
 */
extern const char str_battle_bark_skin[];

/**
 * "Bolts of lighting fall!"
 */
extern const char str_battle_lightning[];

/**
 * "Radiant blue light falls upon you..."
 */
extern const char str_battle_heal[];

/**
 * "Locusts swarm your enemies!"
 */
extern const char str_battle_insect_plague[];

/**
 * "You surge with vitality!"
 */
extern const char str_battle_regen[];

/**
 * "%monster %c attacks!"
 */
extern const char str_battle_monster_attack[];

/**
 * "But they miss!"
 */
extern const char str_battle_monster_miss[];

/**
 * "But it has no effect!"
 */
extern const char str_battle_monster_magic_miss[];

/**
 * "You take %damage damage!"
 */
extern const char str_battle_monster_hit[];

/**
 * "You take %damage %aspect damage!"
 */
extern const char str_battle_monster_hit_aspect[];

/**
 * "But you're completely immune!"
 */
extern const char str_battle_monster_hit_immune[];

/**
 * "You resist, only %damage damage"
 */
extern const char str_battle_monster_hit_resist[];

/**
 * "It's SUPER BAD! %damage damage!"
 */
extern const char str_battle_monster_hit_vuln[];

/**
 * "CRITICAL HIT! You take %damage damage!"
 */
extern const char str_battle_monster_hit_crit[];

/**
 * "%monster %c makes a run for it..."
 */
extern const char str_battle_monster_flee[];

/**
 * "But they cannot get away!"
 */
extern const char str_battle_monster_flee_failure[];

/**
 * "And they get away!"
 */
extern const char str_battle_monster_flee_success[];

/**
 * "%monster %c shivers in fear..."
 */
extern const char str_battle_monster_scared_frozen[];

/**
 * "But you miss!"
 */
extern const char str_battle_player_miss[];

/**
 * "But it has no effect!"
 */
extern const char str_battle_player_magic_miss[];

/**
 * "You deal %damage damage!"
 */
extern const char str_battle_player_hit[];

/**
 * "They're completely immune!"
 */
extern const char str_battle_player_hit_immune[];

/**
 * "They resist your attack, only %damage damage..."
 */
extern const char str_battle_player_hit_resist[];

/**
 * "It's SUPER EFFECTIVE dealing %damage damage!"
 */
extern const char str_battle_player_hit_vuln[];

/**
 * "CRITICAL HIT! You deal %damage damage!"
 */
extern const char str_battle_player_hit_crit[];

/**
 * "You heal %damage HP."
 */
extern const char str_battle_player_heal[];

/**
 * "CRITICAL! You heal a whopping %damage HP!"
 */
extern const char str_battle_player_heal_crit[];

/**
 * "You only heal a measly %damage HP."
 */
extern const char str_battle_player_heal_fumble[];

/**
 * "A COMPLETE WHIFF."
 */
extern const char str_battle_player_miss_all[];

/**
 * "You attempt to flee..."
 */
extern const char str_battle_player_flee_attempt[];

/**
 * "And get away!"
 */
extern const char str_battle_player_flee_success[];

/**
 * "But are blocked!"
 */
extern const char str_battle_player_flee_failure[];

/**
 * "Victory! You gain %exp XP!"
 */
extern const char str_battle_victory[];

/**
 * "Victory! You gain no XP.\fMust have been too weak..."
 */
extern const char str_battle_victory_no_xp[];

/**
 * "LEVEL UP! You reach %level!"
 */
extern const char str_battle_level_up[];

/**
 * "Dummy %c just stands there..."
 */
extern const char str_monster_dummy_pre[];

/**
 * "Sad and wishing for it all to end."
 */
extern const char str_monster_dummy_post[];

/**
 * "I will never die..."
 */
extern const char str_monster_dummy_post_heal[];

/**
 * "Kobold %c raises a tiny axe..."
 */
extern const char str_monster_kobold_axe[];

/**
 * "Kobold %c spits a glob of fire..."
 */
extern const char str_monster_kobold_fire[];

/**
 * "Kobold %c looks dazed..."
 */
extern const char str_monster_kobold_dazed[];

/**
 * "And does nothing!"
 */
extern const char str_monster_kobold_does_nothing[];

/**
 * "But instead it falls back and hiccups!"
 */
extern const char str_monster_kobold_miss[];

#endif
