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
 * "Kobold"
 */
extern const char str_misc_kobold[];

/**
 * "Wilbur"
 */
extern const char str_misc_wilbur[];

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
 * "Victory! You gain %exp XP!"
 */
extern const char str_battle_victory[];

/**
 * "LEVEL UP! You reach %level!"
 */
extern const char str_battle_level_up[];

/**
 * "You attack with your tiny claws..."
 */
extern const char str_summon_kobold_attack[];

/**
 * "You fling a ball of acid..."
 */
extern const char str_summon_kobold_magic_attack[];

/**
 * "Vine Whip"
 */
extern const char str_summon_vine_whip[];

/**
 * ":regen:Mend"
 */
extern const char str_summon_mend[];

/**
 * ":defdown:Summon Ants"
 */
extern const char str_summon_summon_ants[];

/**
 * ":poison:Drity Fang"
 */
extern const char str_summon_dirty_fang[];

/**
 * "You rush forward, fangs bared!"
 */
extern const char str_summon_dirty_fang_attack[];

/**
 * "Your bite deals %damage damage and poison them!"
 */
extern const char str_summon_dirty_fang_hit_poison[];

/**
 * "Your filthy kobold bite deals %damage damage!"
 */
extern const char str_summon_dirty_fang_hit_no_poison[];

/**
 * "Beholder"
 */
extern const char str_summon_beholder[];

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
