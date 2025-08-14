/*
 * strings.js
 *
 * Defines and exports a series of namespaced strings meant to be preformatted
 * and transpiled into C files for inclusion into the ROM.
 */

/**
 * Namespaces to export.
 */
const namespaces = {};

/**
 * Adds a string namespace. Each namespace generates a separate banked C file
 * that defines the formatted string constants.
 * @param {string} name Name for the namespace.
 * @param {number} bank ROM bank in which to write the strings.
 * @param {object} strings Key value pairs of names and string values.
 */
function addNamespace(name, bank, strings) {
  namespaces[name] = { bank, strings };
}

addNamespace('misc', 0, {
  'empty': 'EMPTY...',
  'physical': '',
  'magical': 'magical',
  'earth': 'earth',
  'water': 'water',
  'air': 'air',
  'fire': 'fire',
  'light': 'light',
  'dark': 'dark',
  'potion':        'Potion',
  'remedy':        'Remedy',
  'ether':         'Ether ',
  'atk_up_potion': "ATK&  ",
  'def_up_potion': 'DEF&  ',
  'elixir':        'Elixir',
  'regen_pot':     'Regen ',
  'haste_pot':     'Haste ',
  'dummy': 'Dummy',
  'kobold': 'Kobold',
  'goblin': 'Goblin',
  'zombie': 'Zombie',
  'bugbear': 'Bugbear',
  'owlbear': 'Owlbear',
  'gelatinous_cube': 'G.Cube',
  'displacer_beast': 'D.Beast',
  'will_o_wisp': 'W.O.Wisp',
  'death_knight': 'D.Knight',
  'mind_flayer': 'M.Flayer',
  'beholder': 'Beholder',
  'dragon': 'Dragon',
  'druid_short':    'DRU',
  'fighter_short':  'FTR',
  'monk_short':     'MNK',
  'sorcerer_short': 'SORC',
  'monster_miss_evaded': 'But you evade!',
})

addNamespace('ability', 0, {
  // Druid Attack & Abilities
  'druid_poison_spray':   'Poison Spray ',
  'druid_cure_wounds':    'Cure Wounds  ',
  'druid_bark_skin':      'Bark Skin    ',
  'druid_lightning':      'Lightning    ',
  'druid_heal':           'Heal         ',
  'druid_insect_plague':  'Insect Plague',
  'druid_regen':          'Regenerate   ',
  // Fighter Attack & Abilities
  'fighter_attack':       'Melee Attack ',
  'fighter_second_wind':  'Second Wind  ',
  'fighter_action_surge': 'Action Surge ',
  'fighter_cleave':       'Cleave       ',
  'fighter_trip_attack':  'Trip Attack  ',
  'fighter_menace':       'Menace       ',
  'fighter_indomitable':  'Indomitable  ',
  // Monk Attack & Abilities
  'monk_attack':          'Unarmed Atk. ',
  'monk_evasion':         'Evasion      ',
  'monk_open_palm':       'Open Palm    ',
  'monk_still_mind':      'Still Mind   ',
  'monk_flurry':          'Flurry       ',
  'monk_diamond_body':    'Diamond Body ',
  'monk_quivering_palm':  'Quiver. Palm ',
  // Sorcerer Attack & Ability
  'sorc_attack':          'Magic Missile',
  'sorc_darkness':        'Darkness     ',
  'sorc_fireball':        'Fireball     ',
  'sorc_haste':           'Haste        ',
  'sorc_sleetstorm':      'Sleetstorm   ',
  'sorc_disintegrate':    'Disintegrate ',
  'sorc_wild_magic':      'Wild Magic   ',
});

addNamespace('battle', 3, {
  'monster_scared_frozen': '%monster %c shivers in fear...',
  'monster_paralyzed': "%monster %c can't move!",
  'monster_poison_death': '%monster %c succumbs to the poison!',
  'monster_confuse_attack_self': 'Confused, %monster %c attacks itself!',
  'monster_confuse_attack_other': 'Confused, %monster %c attacks any ally!',
  'monster_confuse_stupor': '%monster %c stares aimlessly.',
  'monster_lies_prone': '%monster %c lies prone!',
  'monster_gets_up': '%monster %c gets up.',
  'monster_ice_slip': '%monster %c slips on the ice!',
  'player_flee_attempt': 'You attempt to flee...',
  'player_flee_success': 'And get away!',
  'player_flee_failure': 'But are blocked!',
  'player_scared': 'You shiver with fear!',
  'player_prone': 'You lie prone!',
  'player_get_up': 'You get up!',
  'player_paralyzed': 'You are paralyzed and cannot move!',
  'player_confused_attack': 'You deal %damage damage to yourself!',
  'player_confused_mumble': 'You mumble some gibberish and giggle a little.',
  'victory': 'Victory! You gain %exp XP!',
  'victory_no_xp': 'Victory! But you gain no XP...',
  'level_up': 'LEVEL UP! Welcome\nto level %level!\n\nHP+%cP restored!',
})

addNamespace('player', 4, {
  // Common
  'miss': 'But you miss!',
  'hit': 'You deal %damage damage!',
  'hit_immune': "They're completely immune!",
  'hit_resist': "They resist your attack, only %damage damage...",
  'hit_vuln': "SUPER EFFECTIVE %damage damage!",
  'hit_crit': 'CRITICAL HIT! You deal %damage damage!',
  'heal_hp': 'You heal %damage HP.',
  'heal_crit': 'CRITICAL! You heal a whopping %damage HP!',
  'heal_fumble': 'You only heal a measly %damage HP.',
  'miss_all': 'A COMPLETE WHIFF.',
  // Druid abilities
  'poison_spray': 'Poison gas erupts from your palm!',
  'cure_wounds': "You're enveloped in blue light...",
  'bark_skin': 'Your skin grows hard as wood.',
  'lightning': 'Bolts of lighting fall!',
  'heal': 'Radiant green light descends...',
  'heal_complete': "You're fully healed!",
  'insect_plague': 'Locusts swarm!',
  'regen': 'You surge with vitality!',
  // Fighter abilities
  'fighter_attack': 'You rush forward!',
  'second_wind': 'You catch your breath...',
  'action_surge': 'You surge forth!',
  'cleave': 'You cleave through your enemies!',
  'trip_attack': 'You sweep your legs low...',
  'trip_attack_hit': 'You topple your foe!',
  'menace': 'You growl menacingly!',
  'indomitable': 'You feel invincible!',
  // Monk abilities
  'monk_attack': 'You strike with your fists!',
  'monk_evasion': 'You feel light on your feet!',
  'monk_open_palm': 'You strike with an open palm!',
  'monk_open_palm_trip': 'You trip %monster %c!',
  'monk_still_mind': 'You become one with the multiverse...',
  'monk_still_mind_post': 'And are healed of all ill effects!',
  'monk_flurry_of_blows': 'You attack with a flurry of blows!',
  'monk_diamond_body': 'You become tough as diamond.',
  'monk_quivering_palm': 'You attack their very essence!',
  'monk_quivering_kill': 'And end them.',
  // Sorcerer abilities
  'sorc_magic_missile_one': 'You fire a magic missile!',
  'sorc_magic_missile': 'You fire %1u magic missiles!',
  'sorc_darkness': 'You enshroud your enemies in darkness!',
  'sorc_fireball': 'EXPLOSION!',
  'sorc_haste': 'You speed up, a lot.',
  'sorc_sleetstorm': 'Sleet rains down!',
  'sorc_disintegrate': 'You send forth a ray of DEATH!',
  'sorc_disintegrate_kill': 'And they are no more.',
  'sorc_wild_magic': 'You let loose a storm of magic!',
  'sorc_wild_magic_fizzle': 'But it fizzles.',
  'sorc_wild_magic_fireball': 'And a fireball goes flying!',
  'sorc_wild_magic_sleetstorm': 'And a sleetstorm descends!',
  // `damage_monster` strings (in player.c)
  'displacer_beast_phase': 'They phase out and evade the attack!',
  'deathknight_revive': 'The deathknight falls, but then revives!',
});

addNamespace('maps', 2, {
  'chest_locked': "The chest is locked.",
  'chest_open': "You opened the chest!",
  'chest_key_locked': "You need a magic key to unlock this chest...",
  'chest_unlock_key': "You unlock the chest with a magic key!",
  'get_magic_key': "You get a magic key!",
  'get_torch': "You find a torch!",
  'already_has_torch': "The chest is empty!",
  'lever_stuck': "It's stuck!",
  'lever_one_way': "Seems this lever was one and done.",
  'door_locked': 'The door is locked.',
  'door_locked_key': 'You need a magic key to unlock this door...',
  'door_unlock_key': 'You unlock the door with a magic key!',
  'sconce_lit_no_torch': 'The sconce burns brightly.',
  'sconce_no_torch': 'Hmm... how do you light this?',
  'sconce_torch_not_lit': 'Your torch lacks a flame.',
  'boss_not_yet': 'Get out of here, runt!',
});

addNamespace('floor_test', 2, {
  'metal_skull': 'This skull is so metal!',
  'glowing_eyes': 'A pair of glowing eyes peers back...',
  'click': 'You hear a click...',
  'creak': 'The other lever creaks.',
  'groan': 'The other lever groans.',
  'chest_click': 'The chest clicks.',
  'no_back': "You cannot return...",
  'door_opens': "The door opens!",
  'growl': "GROWL!",
  'healed': 'You are fully restored!',
})

addNamespace('floor1', 2, {
  'sign_monster_no_fire': 'Monsters fear fire.',
  'sign_empty_chest': "It's empty...",
  'sign_tunnel_cave_in': 'The tunnel has collapsed behind you!',
  'sign_skull_out_of_place': 'This skull seems out of place...',
  'sign_hidden_passage_hint': 'Check behind you...',
  'sign_missing_elite': 'A powerful foe once lived here.',
  'boss_defeated': "Yawp! You won't beat my friends below!",
});

addNamespace('floor2', 2, {
  'sign_items_room': 'Soon you will have to choose.',
  'sign_levers': 'Levers change many things.',
  'door_opens': 'Somewhere a door opens...',
  'elite_msg': "An adventurer? Come, let's test your mettle!",
  'boss_msg': 'SCREEEEEECH!',
});

addNamespace('floor3', 2, {
  'choose_wisely': 'Choose, but choose wisely...',
  'brains': 'BRAIINNNNSS...',
  'boss': 'Jiggle Jiggle... Jiggle...',
  'boss_not_yet': 'Jiggle?',
});

addNamespace('floor4', 2, {
  'elite_attack': 'KWAAAAAAHHH!',
  'boss': 'NyaAAAHHHH!',
  'boss_not_yet': 'Nya?',
});

addNamespace('floor5', 2, {
  'demands': 'The Dragon demands a rainbow...',
  'secrets': 'The walls have secrets...',
  'elite_attack': 'JIGGLE!',
  'boss': 'Come and meet DEATH!',
  'boss_not_yet': 'You are... unworthy.',
});

addNamespace('floor6', 2, {
  'elite_attack': 'BZZZTTT!',
  'boss': 'OH! What a delicious brain!',
  'boss_not_yet': 'You have yet to ripen...',
});

addNamespace('floor7', 2, {
  'riddle': 'Only those unseen may pass...',
  'elite_attack': 'HISSSSS!',
  'boss': 'STARING INTENSIFIES',
  'boss_not_yet': 'EYEBROW RAISES'
});

addNamespace('floor8', 2, {
  'boss': 'Finally, I have awaited this...',
  'elite': 'STARING EVEN MORE',
  'healing_mirror': 'You look in the mirror and your wounds vanish!',
  'healing_mirror_none': 'The mirror has lost its luster...',
});

addNamespace('floor_common', 2, {
  'growl': "GROWL!",
  'light_fires': "Light these fires to open this door!",
  'missing': "Something used to have been here...",
  'no_return': "There is no going back!",
  'steve_jobs': "It's so sad that Steve Jobs Died of Ligma...",
  'tbd': "Placeholders are a big no-no in game development!",
  'new_ability': "You get an ability!",
  'fight_me': "Fight Me!",
  'love': "I LOVE YOU!",
  'strange_wind': "You feel a strong breeze from the north.",
})

function grant_ability_str(name, cast=true) {
  return cast ?
    `You learn to cast ${name}!` :
    `You learn to use ${name}!`
}

addNamespace('gain_ability', 2, {
  'druid1': grant_ability_str('Bark Skin'),
  'druid2': grant_ability_str('Lightning'),
  'druid3': grant_ability_str('Heal'),
  'druid4': grant_ability_str('Insect Plague'),
  'druid5': grant_ability_str('Regenerate'),
  'fighter1': grant_ability_str('Action Surge', false),
  'fighter2': grant_ability_str('Cleave', false),
  'fighter3': grant_ability_str('Trip Attack', false),
  'fighter4': grant_ability_str('Menace', false),
  'fighter5': grant_ability_str('Indomitable', false),
  'monk1': grant_ability_str('Open Palm', false),
  'monk2': grant_ability_str('Still Mind', false),
  'monk3': grant_ability_str('Flurry', false),
  'monk4': grant_ability_str('Diamond Body', false),
  'monk5': grant_ability_str('Quivering Palm', false),
  'sorcerer1': grant_ability_str('Fireball'),
  'sorcerer2': grant_ability_str('Haste'),
  'sorcerer3': grant_ability_str('Sleetstorm'),
  'sorcerer4': grant_ability_str('Disintegrate'),
  'sorcerer5': grant_ability_str('Wild Magic'),
});

addNamespace('chest_item', 2, {
  '2pot_1eth': 'You get 2 potions and an ether!',
  '1pot': 'You get a potion!',
  '1pots': 'You get a potions!',
  'haste_pot': 'You get a haste potion!',
  'regen_pot': 'You get a regen potion!',
  '3regen': 'You get 3 regens!',
  '2pots': 'You get 2 potions!',
  '1eth': 'You get an ether!',
  '1eths': 'You get an ethers!',
  '1remedy': 'You get a remedy!',
  '3potions': 'You get 3 potions!',
  '3ethers': 'You get 3 ethers!',
  '1elixir': 'You get an elixir!',
  '1atkup_1defup': 'You get an ATK& and DEF&!',
  '3elixirs': 'You get 3 elixirs!',
  '3haste': 'You get 3 haste potions!',
});

addNamespace('items', 3, {
  'use_potion': '%damage HP healed!',
  'use_ether_sp': '%damage SP restored!',
  'use_ether_mp': '%damage MP restored!',
  'use_remedy': 'You remedy what ails you!',
  'use_atkup': 'Your attack increases!',
  'use_defup': 'Your defense increases!',
  'use_elixir': 'You fully heal!',
  'use_regen': 'You begin regenerating!',
  'use_haste': 'The world slows down!',
  'use_failed': "The item didn't work!",
})

addNamespace('monster', 6, {
  'flee': '%monster %c makes a run for it...',
  'flee_failure': 'But they cannot get away!',
  'flee_success': 'And they get away!',
  'attack': '%monster %c attacks!',
  'miss': 'But they miss!',
  'magic_miss': 'But it has no effect!',
  'hit': 'You take %damage damage!',
  'hit_aspect': 'You take %damage %aspect damage!',
  'hit_immune': "But you're completely immune!",
  'hit_resist': 'You resist, only %damage damage',
  'hit_vuln': "It's SUPER BAD! %damage damage!",
  'hit_crit': 'CRITICAL HIT! You take %damage damage!',
  'hit_barkskin': 'Your barkskin protects you! %damage damage.',
  'does_nothing': '%monster %c does nothing.',
  'dummy_pre': 'Dummy %c stands still.',
  'dummy_post_heal': '"I will never die..."',
  // Kobold Specials
  'kobold_axe': 'Kobold %c raises a tiny axe...',
  'kobold_fire': 'Kobold %c spits a glob of fire...',
  'kobold_dazed': 'Kobold %c looks dazed...',
  'kobold_does_nothing': 'And does nothing!',
  'kobold_miss': 'But instead it falls over and hiccups!',
  'kobold_get_up': 'Kobold %c gets up.',
  // Goblin specials
  'goblin_nose_pick': 'Goblin %c picks its nose.',
  'goblin_attack': "Goblin %c swings a shortsword!",
  'goblin_acid_arrow': "Goblin %c shoots an acid arrow!",
  // Zombie specials
  'zombie_brains': 'Hrrnng... brains.',
  'zombie_bite_miss': "Zombie %c's bite barely misses!",
  'zombie_bite_hit': "Zombie %c bites and poisons you!",
  'zombie_slam': "Zombie %c swipes at you!",
  // Bugbear special
  'bugbear_for_hruggek': 'Bugbear %c screams "FOR HRUGGEK!"',
  'bugbear_for_hruggek_hit': 'You shiver with fear!',
  'bugbear_for_hruggek_miss': 'You are unimpressed.',
  'bugbear_javelin': 'Bugbear %c throws a javelin!',
  'bugbead_club': 'Bugbear %c swings a club!',
  // Owlbear special
  'owlbear_pounce': 'Owlbear %c pounces!',
  'owlbear_pounce_topple': 'You take %damage damage and fall prone!',
  'owlbear_pounce_miss': 'You barely jump out of the way!',
  'owlbear_multi': 'Owlbear %c tears at you with beak and claw!',
  'owlbear_beak': 'Owlbear %c nips at you!',
  // G. Cube specials
  'gcube_search': 'Gelatinous Cube %c sends out feelers...',
  'gcube_paralyze': 'You are engulfed and paralyzed!',
  'gcube_poison': 'You are engulfed and poisoned!',
  'gcube_engulf_fail': 'You dodge as Gelatinous Cube %c tries to engulf you!',
  'gcube_attack': 'Gelatinous Cube %c swipes at you!',
  // Displacer Beast specials
  'displacer_beast_tentacle': 'Displacer Beast %c strikes with its tentacles!',
  'displacer_beast_2hit': 'They hit twice for %damage damage!',
  'displacer_beast_1hit': 'They hit once for %damage damage!',
  'displacer_beast_miss': 'But they miss with both tentacles!',
  // Will-o-wisp specials
  'will_o_wisp_lightning': 'Will-o-wisp %c sends lightning forth...',
  'will_o_wisp_scare': 'Will-o-wisp %c passes through you!',
  'will_o_wisp_scare_hit': 'Terror fills your soul!',
  'will_o_wisp_scare_miss': 'But you hold fast!',
  'will_o_wisp_siphon': 'Will-o-wisp %c siphons your soul...',
  'will_o_wisp_siphon_hit': 'They steal %damage HP!',
  'will_o_wisp_hit': 'They shock you for %damage damage!',
  // Deathknight Specials
  'deathknight_attack': 'Death Knight %c swings their longsword...',
  'deathknight_hit1': 'They slash you for %damage damage!',
  'deathknight_hit2': 'They hit twice! %damage damage!',
  'deathknight_hellfire': 'Death Knight %c sends forth a hellfire orb!',
  'deathknight_hellfire_hit': 'Direct hit! You take %damage damage!',
  'deathknight_hellfire_miss': 'You dodge! But still take %damage damage!',
});

addNamespace('monster2', 7, {
  'does_nothing': '%monster %c does nothing.',
  'monster_miss': 'But they miss!',
  // Mindflayer special
  'mindflayer_mind_blast':        'Mind Flayer %c emits a wave of psychic energy!',
  'mindflayer_mind_blast_miss':   'But you resist!',
  'mindflayer_mind_blast_hit':    'You take %damage damage, and are confused!',
  'mindflayer_tentacle':          'Mind Flayer %c lashes out with its tentacles!',
  'mindflayer_extract_brain':     'Mind Flayer %c attempts to eat your brain!',
  'mindflayer_extract_brain_hit': 'Your brain is gobbled up!',
  // Beholder special
  'beholder_bite': 'Beholder %c chomps at you...',
  'beholder_shoot_ray': 'Beholder %c shoots a ray from an eyestalk...',
  'beholder_ray_paralyze': 'You take %damage damage and are paralyzed!',
  'beholder_ray_fear': 'You take %damage damage and you feel dread!',
  'beholder_ray_slow': 'You take %damage damage and you slow down!',
  'beholder_ray_necro': 'You take %damage damage and are poisoned!',
  'beholder_ray_trip': 'You take %damage damage and fall prone!',
  'beholder_ray_death': 'Your soul escapes your body...',
  'beholder_ray_miss': 'But you dodge the ray!',
  'beholder_bite_miss': 'But their bite misses!',
  'beholder_ray_resist': 'But you resist the ray!',
  // Dragon Specials
  'dragon_attack': 'Dragon %c swoops down and attacks!',
  'dragon_miss': 'But their attacks miss!',
  'dragon_hit_triple': 'They hit THREE TIMES for %damage damage!',
  'dragon_hit_double': 'They hit TWICE for %damage damage!',
  'dragon_hit_single': 'They hit for %damage damage!',
  'dragon_legendary_tail': 'Dragon %c sweeps its tail!',
  'dragon_legendary_tail_miss': 'But you dodge out of the way!',
  'dragon_legendary_wing': 'Dragon %c beats its wings!',
  'dragon_legendary_wing_miss': 'But you take cover!',
  'dragon_legendary_wing_hit': 'You are toppled and take %damage damage!',
  'dragon_fright': 'Dragon %c towers above you!',
  'dragon_fright_miss': 'But your resolve does not waiver!',
  'dragon_fright_hit': 'And you fear for your life!',
  'dragon_fire_breath': 'Dragon %c exhales a wave of fire!',
  'dragon_fire_breath_miss': 'You dodge, but still take %damage damage!',
})

addNamespace('credits', 1, {
  //           X123456789012345678X
  'story1_1': 'With a last gasp',
  'story1_2': 'and puff of smoke',
  'story1_3': 'The dragon was',
  'story1_4': 'Defeated...',

  'story2_1': 'The quest done,',
  'story2_2': 'the hero ascended',
  'story2_3': 'an ancient',
  'story2_4': 'staircase.',

  'story3_1': 'And at long last...',
  'story3_2': 'was free of the',
  'story3_3': 'The dungeon and',
  'story3_4': '   the dragon.',

  'developed_by': 'DEVELOPED BY',
  'programming': 'PROGRAMMING',
  'game_design': 'GAME DESIGN',
  'monster_art': 'MONSTER ART',
  'title_art': 'TITLE ART',
  'dungeon_art': 'DUNGEON ART',

  'neshacker': 'NESHACKER',
  'ryan': 'Ryan Richards',
  'tommy': 'Tommy',
  'mono': 'Mono',
  'ledu': 'Ledu',
  'patreon': 'SPECIAL THANKS',
  'nh_patreon': 'NesHacker Patreon',
  'nh_patreon2': 'patreon.com/',
  'nh_patreon3': '   NesHacker',

  'thank_you': 'Thank you',
  'for_playing': 'for playing!',
});

// Export the namespaces
module.exports = namespaces;
