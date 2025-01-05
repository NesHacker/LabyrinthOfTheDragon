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
 * "DRU"
 */
extern const char str_misc_druid_short[];

/**
 * "FTR"
 */
extern const char str_misc_fighter_short[];

/**
 * "MNK"
 */
extern const char str_misc_monk_short[];

/**
 * "SORC"
 */
extern const char str_misc_sorcerer_short[];

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
 * "Melee Attack "
 */
extern const char str_ability_fighter_attack[];

/**
 * "Second Wind  "
 */
extern const char str_ability_fighter_second_wind[];

/**
 * "Action Surge "
 */
extern const char str_ability_fighter_action_surge[];

/**
 * "Cleave       "
 */
extern const char str_ability_fighter_cleave[];

/**
 * "Trip Attack  "
 */
extern const char str_ability_fighter_trip_attack[];

/**
 * "Menace       "
 */
extern const char str_ability_fighter_menace[];

/**
 * "Indomitable  "
 */
extern const char str_ability_fighter_indomitable[];

/**
 * "Unarmed Atk. "
 */
extern const char str_ability_monk_attack[];

/**
 * "Evasion      "
 */
extern const char str_ability_monk_evasion[];

/**
 * "Open Palm    "
 */
extern const char str_ability_monk_open_palm[];

/**
 * "Still Mind   "
 */
extern const char str_ability_monk_still_mind[];

/**
 * "Flurry       "
 */
extern const char str_ability_monk_flurry[];

/**
 * "Diamond Body "
 */
extern const char str_ability_monk_diamond_body[];

/**
 * "Quiver. Palm "
 */
extern const char str_ability_monk_quivering_palm[];

/**
 * "Magic Missle "
 */
extern const char str_ability_sorc_attack[];

/**
 * "Darkness     "
 */
extern const char str_ability_sorc_darkness[];

/**
 * "Fireball     "
 */
extern const char str_ability_sorc_fireball[];

/**
 * "Haste        "
 */
extern const char str_ability_sorc_haste[];

/**
 * "Sleetstorm   "
 */
extern const char str_ability_sorc_sleetstorm[];

/**
 * "Disintegrate "
 */
extern const char str_ability_sorc_disintigrate[];

/**
 * "Wild Magic   "
 */
extern const char str_ability_sorc_wild_magic[];

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
 * "%monster %c lies prone!"
 */
extern const char str_battle_monster_lies_prone[];

/**
 * "%monster %c gets up."
 */
extern const char str_battle_monter_gets_up[];

/**
 * "But you evade!"
 */
extern const char str_battle_monster_miss_evaded[];

/**
 * "%monster %c slips on the ice!"
 */
extern const char str_battle_monster_ice_slip[];

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
 * "You shiver with fear!"
 */
extern const char str_battle_player_scared[];

/**
 * "You lie prone!"
 */
extern const char str_battle_player_prone[];

/**
 * "You get up!"
 */
extern const char str_battle_player_get_up[];

/**
 * "You are paralyzed and cannot move!"
 */
extern const char str_battle_player_paralyzed[];

/**
 * "You deal %damage damage to yourself!"
 */
extern const char str_battle_player_confused_attack[];

/**
 * "You mumble some gibberish and giggle a little."
 */
extern const char str_battle_player_confused_mumble[];

/**
 * "Victory! You gain %exp XP!"
 */
extern const char str_battle_victory[];

/**
 * "Victory! But you gain no XP..."
 */
extern const char str_battle_victory_no_xp[];

/**
 * "LEVEL UP! You reach %level!"
 */
extern const char str_battle_level_up[];

/**
 * "But you miss!"
 */
extern const char str_player_miss[];

/**
 * "You deal %damage damage!"
 */
extern const char str_player_hit[];

/**
 * "They're completely immune!"
 */
extern const char str_player_hit_immune[];

/**
 * "They resist your attack, only %damage damage..."
 */
extern const char str_player_hit_resist[];

/**
 * "SUPER EFFECTIVE %damage damage!"
 */
extern const char str_player_hit_vuln[];

/**
 * "CRITICAL HIT! You deal %damage damage!"
 */
extern const char str_player_hit_crit[];

/**
 * "Radiant green light descends..."
 */
extern const char str_player_heal[];

/**
 * "CRITICAL! You heal a whopping %damage HP!"
 */
extern const char str_player_heal_crit[];

/**
 * "You only heal a measly %damage HP."
 */
extern const char str_player_heal_fumble[];

/**
 * "A COMPLETE WHIFF."
 */
extern const char str_player_miss_all[];

/**
 * "Poison gas erupts from your palm!"
 */
extern const char str_player_poison_spray[];

/**
 * "You're enveloped in blue light..."
 */
extern const char str_player_cure_wounds[];

/**
 * "Your skin grows hard as wood."
 */
extern const char str_player_bark_skin[];

/**
 * "Bolts of lighting fall!"
 */
extern const char str_player_lightning[];

/**
 * "You're fully healed!"
 */
extern const char str_player_heal_complete[];

/**
 * "Locusts swarm!"
 */
extern const char str_player_insect_plague[];

/**
 * "You surge with vitality!"
 */
extern const char str_player_regen[];

/**
 * "You rush forward!"
 */
extern const char str_player_fighter_attack[];

/**
 * "You heal %damage HP!"
 */
extern const char str_player_second_wind[];

/**
 * "You surge forth!"
 */
extern const char str_player_action_surge[];

/**
 * "You cleave through your enemies!"
 */
extern const char str_player_cleave[];

/**
 * "You sweep your legs low..."
 */
extern const char str_player_trip_attack[];

/**
 * "You topple your foe!"
 */
extern const char str_player_trip_attack_hit[];

/**
 * "You growl menacingly!"
 */
extern const char str_player_menace[];

/**
 * "You feel invincible!"
 */
extern const char str_player_indomitable[];

/**
 * "You strike with your fists!"
 */
extern const char str_player_monk_attack[];

/**
 * "You feel light on your feet!"
 */
extern const char str_player_monk_evasion[];

/**
 * "You strike with an open palm!"
 */
extern const char str_player_monk_open_palm[];

/**
 * "You trip %monster %c!"
 */
extern const char str_player_monk_open_palm_trip[];

/**
 * "You become one with the multiverse..."
 */
extern const char str_player_monk_still_mind[];

/**
 * "And are healed of all ill effects!"
 */
extern const char str_player_monk_still_mind_post[];

/**
 * "You attack with a flurry of blows!"
 */
extern const char str_player_monk_flurry_of_blows[];

/**
 * "You become tough as diamond."
 */
extern const char str_player_monk_diamond_body[];

/**
 * "You attack their very essence!"
 */
extern const char str_player_monk_quivering_palm[];

/**
 * "And end them."
 */
extern const char str_player_monk_quivering_kill[];

/**
 * "You fire a magic missle!"
 */
extern const char str_player_sorc_magic_missile_one[];

/**
 * "You fire %1u magic missiles!"
 */
extern const char str_player_sorc_magic_missile[];

/**
 * "You enshroud your enemies in darkness!"
 */
extern const char str_player_sorc_darkness[];

/**
 * "EXPLOSSION!"
 */
extern const char str_player_sorc_fireball[];

/**
 * "You speed up, a lot."
 */
extern const char str_player_sorc_haste[];

/**
 * "Sleet rains down!"
 */
extern const char str_player_sorc_sleetstorm[];

/**
 * "You send forth a ray of DEATH!"
 */
extern const char str_player_sorc_disintegrate[];

/**
 * "And they are no more."
 */
extern const char str_player_sorc_disintegrate_kill[];

/**
 * "You let loose a storm of magic!"
 */
extern const char str_player_sorc_wild_magic[];

/**
 * "But it fizzles."
 */
extern const char str_player_sorc_wild_magic_fizzle[];

/**
 * "And a fireball goes flying!"
 */
extern const char str_player_sorc_wild_magic_fireball[];

/**
 * "And a sleetstorm descends!"
 */
extern const char str_player_sorc_wild_magic_sleetstorm[];

/**
 * "They phase out and evade the attack!"
 */
extern const char str_player_displacer_beast_phase[];

/**
 * "The deathknight falls, but then revives!"
 */
extern const char str_player_deathknight_revive[];

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
 * "Monsters fear fire!"
 */
extern const char str_maps_sign_monster_no_fire[];

/**
 * "This skull is so metal!"
 */
extern const char str_floor_test_metal_skull[];

/**
 * "A pair of glowing eyes peers back..."
 */
extern const char str_floor_test_glowing_eyes[];

/**
 * "You hear a click..."
 */
extern const char str_floor_test_click[];

/**
 * "The other lever creaks."
 */
extern const char str_floor_test_creak[];

/**
 * "The other lever groans."
 */
extern const char str_floor_test_groan[];

/**
 * "The chest clicks."
 */
extern const char str_floor_test_chest_click[];

/**
 * "You cannot return..."
 */
extern const char str_floor_test_no_back[];

/**
 * "The door opens!"
 */
extern const char str_floor_test_door_opens[];

/**
 * "GROWL!"
 */
extern const char str_floor_test_growl[];

/**
 * "GROWL!"
 */
extern const char str_floor_common_growl[];

/**
 * "Light these fires to open this door!"
 */
extern const char str_floor_common_light_fires[];

/**
 * "Something used to have been here..."
 */
extern const char str_floor_common_missing[];

/**
 * "There is no going back!"
 */
extern const char str_floor_common_no_return[];

/**
 * "It's so sad that Steve Jobs Died of Ligma..."
 */
extern const char str_floor_common_steve_jobs[];

/**
 * "Placeholders are a big no-no in game development!"
 */
extern const char str_floor_common_tbd[];

/**
 * "You get an ability!"
 */
extern const char str_floor_common_new_ability[];

/**
 * "Fight Me!"
 */
extern const char str_floor_common_fight_me[];

/**
 * "I LOVE YOU!"
 */
extern const char str_floor_common_love[];

/**
 * "You feel a strong breeze from the north."
 */
extern const char str_floor_common_strange_wind[];

/**
 * "Levers below open these doors."
 */
extern const char str_floor_three_lever_puzzle[];

/**
 * "Nine to the boss, Five to a good fight"
 */
extern const char str_floor_three_lever_one[];

/**
 * "Try them all if you have a key"
 */
extern const char str_floor_three_lever_two[];

/**
 * "You are fully restored!"
 */
extern const char str_floor_test_healed[];

/**
 * "You get 2 potions and an ether!"
 */
extern const char str_chest_item_2pot_1eth[];

/**
 * "You get a potion!"
 */
extern const char str_chest_item_1pot[];

/**
 * "You get a potions!"
 */
extern const char str_chest_item_1pots[];

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
 * "Your barkskin protects you! %damage damage."
 */
extern const char str_monster_hit_barkskin[];

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
 * "But instead it falls over and hiccups!"
 */
extern const char str_monster_kobold_miss[];

/**
 * "Kobold %c gets up."
 */
extern const char str_monster_kobold_get_up[];

/**
 * "Goblin %c picks its nose."
 */
extern const char str_monster_goblin_nose_pick[];

/**
 * "Goblin %c swings a shortsword!"
 */
extern const char str_monster_goblin_attack[];

/**
 * "Goblin %c shoots an acid arrow!"
 */
extern const char str_monster_goblin_acid_arrow[];

/**
 * "Hrrnng... brains."
 */
extern const char str_monster_zombie_brains[];

/**
 * "Zombie %c's bite barely misses!"
 */
extern const char str_monster_zombie_bite_miss[];

/**
 * "Zombie %c bites and poisons you!"
 */
extern const char str_monster_zombie_bite_hit[];

/**
 * "Zombie %c swipes at you!"
 */
extern const char str_monster_zombie_slam[];

/**
 * "Bugbear %c screams "FOR HRUGGEK!""
 */
extern const char str_monster_bugbear_for_hruggek[];

/**
 * "You shiver with fear!"
 */
extern const char str_monster_bugbear_for_hruggek_hit[];

/**
 * "You are unimpressed."
 */
extern const char str_monster_bugbear_for_hruggek_miss[];

/**
 * "Bugbear %c throws a javelin!"
 */
extern const char str_monster_bugbear_javelin[];

/**
 * "Bugbear %c swings a club!"
 */
extern const char str_monster_bugbead_club[];

/**
 * "Owlbear %c pounces!"
 */
extern const char str_monster_owlbear_pounce[];

/**
 * "You take %damage damage and fall prone!"
 */
extern const char str_monster_owlbear_pounce_topple[];

/**
 * "You barely jump out of the way!"
 */
extern const char str_monster_owlbear_pounce_miss[];

/**
 * "Owlbear %c tears at you with beak and claw!"
 */
extern const char str_monster_owlbear_multi[];

/**
 * "Owlbear %c nips at you!"
 */
extern const char str_monster_owlbear_beak[];

/**
 * "Gelatinous Cube %c sends out feelers..."
 */
extern const char str_monster_gcube_search[];

/**
 * "You are engulfed and paralyzed!"
 */
extern const char str_monster_gcube_paralyze[];

/**
 * "You are engulfed and poisoned!"
 */
extern const char str_monster_gcube_poison[];

/**
 * "You dodge as Gelatinous Cube %c tries to engulf you!"
 */
extern const char str_monster_gcube_engulf_fail[];

/**
 * "Gelatinous Cube %c swipes at you!"
 */
extern const char str_monster_gcube_attack[];

/**
 * "Displacer Beast %c strikes with its tentacles!"
 */
extern const char str_monster_displacer_beast_tentacle[];

/**
 * "They hit twice for %damage damage!"
 */
extern const char str_monster_displacer_beast_2hit[];

/**
 * "They hit once for %damage damage!"
 */
extern const char str_monster_displacer_beast_1hit[];

/**
 * "But they miss with both tentalces!"
 */
extern const char str_monster_displacer_beast_miss[];

/**
 * "Will-o-wisp %c sends lightning forth..."
 */
extern const char str_monster_will_o_wisp_lightning[];

/**
 * "Will-o-wisp %c passes through you!"
 */
extern const char str_monster_will_o_wisp_scare[];

/**
 * "Terror fills your soul!"
 */
extern const char str_monster_will_o_wisp_scare_hit[];

/**
 * "But you hold fast!"
 */
extern const char str_monster_will_o_wisp_scare_miss[];

/**
 * "Will-o-wisp %c siphons your soul..."
 */
extern const char str_monster_will_o_wisp_siphon[];

/**
 * "They steal %damage HP!"
 */
extern const char str_monster_will_o_wisp_siphon_hit[];

/**
 * "They shock you for %damage damage!"
 */
extern const char str_monster_will_o_wisp_hit[];

/**
 * "Death Knight %c swings their longsword..."
 */
extern const char str_monster_deathknight_attack[];

/**
 * "They slash you for %damage damage!"
 */
extern const char str_monster_deathknight_hit1[];

/**
 * "They hit twice! %damage damage!"
 */
extern const char str_monster_deathknight_hit2[];

/**
 * "Death Knight %c sends forth a hellfire orb!"
 */
extern const char str_monster_deathknight_hellfire[];

/**
 * "Direct hit! You take %damage damage!"
 */
extern const char str_monster_deathknight_hellfire_hit[];

/**
 * "You dodge! But still take %damage damage!"
 */
extern const char str_monster_deathknight_hellfire_miss[];

/**
 * "%monster %c does nothing."
 */
extern const char str_monster2_does_nothing[];

/**
 * "Mind Flayer %c emits a wave of psychic energy!"
 */
extern const char str_monster2_mindflayer_mind_blast[];

/**
 * "But you resist!"
 */
extern const char str_monster2_mindflayer_mind_blast_miss[];

/**
 * "You take %damage damage, and are confused!"
 */
extern const char str_monster2_mindflayer_mind_blast_hit[];

/**
 * "Mind Flayer %c lashes out with its tentacles!"
 */
extern const char str_monster2_mindflayer_tentacle[];

/**
 * "Mind Flayer %c attempts to eat your brain!"
 */
extern const char str_monster2_mindflayer_extract_brain[];

/**
 * "Your brain is gobbled up!"
 */
extern const char str_monster2_mindflayer_extract_brain_hit[];

/**
 * "Beholder %c chomps at you..."
 */
extern const char str_monster2_beholder_bite[];

/**
 * "Beholder %c shoots a ray from an eyestalk..."
 */
extern const char str_monster2_beholder_shoot_ray[];

/**
 * "You take %damage damage and are paralyzed!"
 */
extern const char str_monster2_beholder_ray_paralyze[];

/**
 * "You take %damage damage and you feel dread!"
 */
extern const char str_monster2_beholder_ray_fear[];

/**
 * "You take %damage damage and you slow down!"
 */
extern const char str_monster2_beholder_ray_slow[];

/**
 * "You take %damage damage and are poisoned!"
 */
extern const char str_monster2_beholder_ray_necro[];

/**
 * "You take %damage damage and fall prone!"
 */
extern const char str_monster2_beholder_ray_trip[];

/**
 * "Your soul escapes your body..."
 */
extern const char str_monster2_beholder_ray_death[];

/**
 * "But you dodge the ray!"
 */
extern const char str_monster2_beholder_ray_miss[];

/**
 * "But their bite misses!"
 */
extern const char str_monster2_beholder_bit_miss[];

/**
 * "Dragon %c swoops down and attacks!"
 */
extern const char str_monster2_dragon_attack[];

/**
 * "But their attacks miss!"
 */
extern const char str_monster2_dragon_miss[];

/**
 * "They hit THREE TIMES for %damage damage!"
 */
extern const char str_monster2_dragon_hit_triple[];

/**
 * "They hit TWICE for %damage damage!"
 */
extern const char str_monster2_dragon_hit_double[];

/**
 * "They hit for %damage damage!"
 */
extern const char str_monster2_dragon_hit_single[];

/**
 * "Dragon %c sweeps its tail!"
 */
extern const char str_monster2_dragon_legendary_tail[];

/**
 * "But you dodge out of the way!"
 */
extern const char str_monster2_dragon_legendary_tail_miss[];

/**
 * "Dragon %c beats its wings!"
 */
extern const char str_monster2_dragon_legendary_wing[];

/**
 * "But you take cover!"
 */
extern const char str_monster2_dragon_legendary_wing_miss[];

/**
 * "You are toppled and take %damage damage!"
 */
extern const char str_monster2_dragon_legendary_wing_hit[];

/**
 * "Dragon %c towers above you!"
 */
extern const char str_monster2_dragon_fright[];

/**
 * "But your resolve does not waiver!"
 */
extern const char str_monster2_dragon_fright_miss[];

/**
 * "And you fear for your life!"
 */
extern const char str_monster2_dragon_fright_hit[];

/**
 * "Dragon %c exhales a wave of fire!"
 */
extern const char str_monster2_dragon_fire_breath[];

/**
 * "You dodge, but still take %damage damage!"
 */
extern const char str_monster2_dragon_fire_breath_miss[];

#endif
