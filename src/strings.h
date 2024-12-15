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
 * "Beholder"
 */
extern const char str_summon_name_beholder[];

/**
 * "Kobold"
 */
extern const char str_summon_name_kobold[];

/**
 * "Wilbur"
 */
extern const char str_summon_name_wilbur[];

/**
 * "Vine Whip"
 */
extern const char str_ability_vine_whip[];

/**
 * ":regen:Mend"
 */
extern const char str_ability_mend[];

/**
 * ":defdown:Summon Ants"
 */
extern const char str_ability_summon_ants[];

/**
 * ":poison:Drity Fang"
 */
extern const char str_ability_dirty_fang[];

/**
 * "You rush forward, fangs bared!"
 */
extern const char str_battle_dirty_fang_attack[];

/**
 * "Your bite deals %damage damage and poison them!"
 */
extern const char str_battle_dirty_fang_hit_poison[];

/**
 * "Your filthy kobold bite deals %damage damage!"
 */
extern const char str_battle_dirty_fang_hit_no_poison[];

/**
 * "You attack with your tiny claws..."
 */
extern const char str_battle_kobold_base_attack[];

/**
 * "You fling a ball of acid..."
 */
extern const char str_battle_kobold_base_magic_attack[];

/**
 * "Kobold %c attacks with a stone axe..."
 */
extern const char str_battle_kobold_axe[];

/**
 * ":fight: %monster %c attacks..."
 */
extern const char str_battle_monster_attack[];

/**
 * ":magic: %monster %c casts a spell..."
 */
extern const char str_battle_monster_magic_attack[];

/**
 * "But they miss!"
 */
extern const char str_battle_monster_miss[];

/**
 * "But the spell goes astray!"
 */
extern const char str_battle_monster_miss_magic[];

/**
 * "They hit for %damage damage!"
 */
extern const char str_battle_monster_hit[];

/**
 * "They hit for %damage %aspect damage!"
 */
extern const char str_battle_monster_hit_aspect[];

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

#endif
