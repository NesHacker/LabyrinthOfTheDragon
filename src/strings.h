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
 * "Potion"
 */
extern const char str_misc_potion[];

/**
 * "Remedy"
 */
extern const char str_misc_remedy[];

/**
 * "Ether "
 */
extern const char str_misc_ether[];

/**
 * "ATK ^ "
 */
extern const char str_misc_atk_up_potion[];

/**
 * "DEF ^ "
 */
extern const char str_misc_def_up_potion[];

/**
 * "Elixer"
 */
extern const char str_misc_elixer[];

/**
 * "Regen "
 */
extern const char str_misc_regen_pot[];

/**
 * "Haste "
 */
extern const char str_misc_haste_pot[];

/**
 * "Dummy"
 */
extern const char str_misc_dummy[];

/**
 * "Kobold"
 */
extern const char str_misc_kobold[];

/**
 * "Goblin"
 */
extern const char str_misc_goblin[];

/**
 * "Zombie"
 */
extern const char str_misc_zombie[];

/**
 * "Bugbear"
 */
extern const char str_misc_bugbear[];

/**
 * "Owlbear"
 */
extern const char str_misc_owlbear[];

/**
 * "G.Cube"
 */
extern const char str_misc_gelatinous_cube[];

/**
 * "D.Beast"
 */
extern const char str_misc_displacer_beast[];

/**
 * "W.O.Wisp"
 */
extern const char str_misc_will_o_wisp[];

/**
 * "D.Knight"
 */
extern const char str_misc_death_knight[];

/**
 * "M.Flayer"
 */
extern const char str_misc_mind_flayer[];

/**
 * "Beholder"
 */
extern const char str_misc_beholder[];

/**
 * "Dragon"
 */
extern const char str_misc_dragon[];

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
 * "%monster %c shivers in fear..."
 */
extern const char str_battle_monster_scared_frozen[];

/**
 * "%monster %c can't move!"
 */
extern const char str_battle_monster_paralyzed[];

/**
 * "%monster %c succumbs to the poison!"
 */
extern const char str_battle_monster_poison_death[];

/**
 * "Confused, %monster %c attacks itself!"
 */
extern const char str_battle_monster_confuse_attack_self[];

/**
 * "Confused, %monster %c attacks any ally!"
 */
extern const char str_battle_monster_confuse_attack_other[];

/**
 * "%monster %c stares aimlessly."
 */
extern const char str_battle_monster_confuse_stupor[];

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
 * "The chest is locked."
 */
extern const char str_maps_chest_locked[];

/**
 * "You opened the chest!"
 */
extern const char str_maps_chest_open[];

/**
 * "The chest was empty."
 */
extern const char str_maps_chest_empty[];

/**
 * "You need a magic key to unlock this chest..."
 */
extern const char str_maps_chest_key_locked[];

/**
 * "You unlock the chest with a magic key!"
 */
extern const char str_maps_chest_unlock_key[];

/**
 * "You get a magic key!"
 */
extern const char str_maps_get_magic_key[];

/**
 * "You find a torch!"
 */
extern const char str_maps_get_torch[];

/**
 * "It's stuck!"
 */
extern const char str_maps_lever_stuck[];

/**
 * "Seems this lever was one and done."
 */
extern const char str_maps_lever_one_way[];

/**
 * "The door is locked."
 */
extern const char str_maps_door_locked[];

/**
 * "You need a magic key to unlock this door..."
 */
extern const char str_maps_door_locked_key[];

/**
 * "You unlock the door with a magic key!"
 */
extern const char str_maps_door_unlock_key[];

/**
 * "The sconce burns brightly."
 */
extern const char str_maps_sconce_lit_no_torch[];

/**
 * "Hmm... how do you light this?"
 */
extern const char str_maps_sconce_no_torch[];

/**
 * "Your torch lacks a flame."
 */
extern const char str_maps_sconce_torch_not_lit[];

/**
 * "You get 2 potions and an ether!"
 */
extern const char str_chest_item_2pot_1eth[];

/**
 * "You get a haste potion!"
 */
extern const char str_chest_item_haste_pot[];

/**
 * "You get a regen potion!"
 */
extern const char str_chest_item_regen_pot[];

/**
 * "%damage HP healed!"
 */
extern const char str_items_use_potion[];

/**
 * "%damage SP restored!"
 */
extern const char str_items_use_ether_sp[];

/**
 * "%damage MP restored!"
 */
extern const char str_items_use_ether_mp[];

/**
 * "You remedy what ails you!"
 */
extern const char str_items_use_remedy[];

/**
 * "Your attack increases!"
 */
extern const char str_items_use_atkup[];

/**
 * "Your defense increases!"
 */
extern const char str_items_use_defup[];

/**
 * "You fully heal!"
 */
extern const char str_items_use_elixer[];

/**
 * "You begin regenerating!"
 */
extern const char str_items_use_regen[];

/**
 * "The world slows down!"
 */
extern const char str_items_use_haste[];

/**
 * "The item didn't work!"
 */
extern const char str_items_use_failed[];

/**
 * "%monster %c makes a run for it..."
 */
extern const char str_monster_flee[];

/**
 * "But they cannot get away!"
 */
extern const char str_monster_flee_failure[];

/**
 * "And they get away!"
 */
extern const char str_monster_flee_success[];

/**
 * "%monster %c attacks!"
 */
extern const char str_monster_attack[];

/**
 * "But they miss!"
 */
extern const char str_monster_miss[];

/**
 * "But it has no effect!"
 */
extern const char str_monster_magic_miss[];

/**
 * "You take %damage damage!"
 */
extern const char str_monster_hit[];

/**
 * "You take %damage %aspect damage!"
 */
extern const char str_monster_hit_aspect[];

/**
 * "But you're completely immune!"
 */
extern const char str_monster_hit_immune[];

/**
 * "You resist, only %damage damage"
 */
extern const char str_monster_hit_resist[];

/**
 * "It's SUPER BAD! %damage damage!"
 */
extern const char str_monster_hit_vuln[];

/**
 * "CRITICAL HIT! You take %damage damage!"
 */
extern const char str_monster_hit_crit[];

/**
 * "%monster %c does nothing."
 */
extern const char str_monster_does_nothing[];

/**
 * "Dummy %c stands still."
 */
extern const char str_monster_dummy_pre[];

/**
 * ""I will never die...""
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
