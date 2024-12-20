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

/**
 * Misc. game text strings used in various places.
 */
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
})

addNamespace('ability', 0, {
  'druid_poison_spray':  'Poison Spray ',
  'druid_cure_wounds':   'Cure Wounds  ',
  'druid_bark_skin':     'Bark Skin    ',
  'druid_ligtning':      'Lightning    ',
  'druid_heal':          'Heal         ',
  'druid_insect_plague': 'Insect Plague',
  'druid_regen':         'Regenerate   ',
});

/**
 * General battle strings
 */
addNamespace('battle', 3, {
  'poison_spray': 'Poison gas erupts from your palm!',
  'cure_wounds': "You're enveloped in blue light...",
  'bark_skin': 'Your skin grows hard as wood.',
  'lightning': 'Bolts of lighting fall!',
  'heal': 'Radiant blue light falls upon you...',
  'insect_plague': 'Locusts swarm your enemies!',
  'regen': 'You surge with vitality!',
  'monster_attack': '%monster %c attacks!',
  'monster_miss': 'But they miss!',
  'monster_magic_miss': 'But it has no effect!',
  'monster_hit': 'You take %damage damage!',
  'monster_hit_aspect': 'You take %damage %aspect damage!',
  'monster_hit_immune': "But you're completely immune!",
  'monster_hit_resist': 'You resist, only %damage damage',
  'monster_hit_vuln': "It's SUPER BAD! %damage damage!",
  'monster_hit_crit': 'CRITICAL HIT! You take %damage damage!',
  'monster_flee': '%monster %c makes a run for it...',
  'monster_flee_failure': 'But they cannot get away!',
  'monster_flee_success': 'And they get away!',
  'monster_scared_frozen': '%monster %c shivers in fear...',
  'monster_paralyzed': "%monster %c can't move!",
  'monster_poison_death': '%monster %c succumbs to the poison!',
  'monster_confuse_attack_self': 'Confused, %monster %c attacks itself!',
  'monster_confuse_attack_other': 'Confused, %monster %c attacks any ally!',
  'monster_confuse_stupor': '%monster %c stares aimlessly.',
  'player_miss': 'But you miss!',
  'player_magic_miss': 'But it has no effect!',
  'player_hit': 'You deal %damage damage!',
  'player_hit_immune': "They're completely immune!",
  'player_hit_resist': "They resist your attack, only %damage damage...",
  'player_hit_vuln': "It's SUPER EFFECTIVE dealing %damage damage!",
  'player_hit_crit': 'CRITICAL HIT! You deal %damage damage!',
  'player_heal': 'You heal %damage HP.',
  'player_heal_crit': 'CRITICAL! You heal a whopping %damage HP!',
  'player_heal_fumble': 'You only heal a measly %damage HP.',
  'player_miss_all': 'A COMPLETE WHIFF.',
  'player_flee_attempt': 'You attempt to flee...',
  'player_flee_success': 'And get away!',
  'player_flee_failure': 'But are blocked!',
  'victory': 'Victory! You gain %exp XP!',
  'victory_no_xp': 'Victory! You gain no XP.\fMust have been too weak...',
  'level_up': 'LEVEL UP! You reach %level!',
})

/**
 * Monster specific battle strings
 */
addNamespace('monster', 3, {
  'dummy_pre': 'Dummy %c just stands there...',
  'dummy_post': 'Sad and wishing for it all to end.',
  'dummy_post_heal': '"I will never die..."',
  'kobold_axe': 'Kobold %c raises a tiny axe...',
  'kobold_fire': 'Kobold %c spits a glob of fire...',
  'kobold_dazed': 'Kobold %c looks dazed...',
  'kobold_does_nothing': 'And does nothing!',
  'kobold_miss': 'But instead it falls back and hiccups!',
})

// Export the namespaces
module.exports = namespaces;
