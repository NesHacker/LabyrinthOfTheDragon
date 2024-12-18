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
  'druid_call_ligtning': 'Call Lit.    ',
  'druid_heal':          'Heal         ',
  'druid_insect_plague': 'Insect Plague',
  'druid_regen':         'Regenerate   ',
});

/**
 * General battle strings
 */
addNamespace('battle', 3, {
  'poison_spray': 'Poison gas erupts from your palm!',
  'monster_miss': 'But they miss!',
  'monster_magic_miss': 'But it has no effect!',
  'monster_hit': 'You take %damage damage!',
  'monster_hit_aspect': 'You take %damage %aspect damage!',
  'monster_hit_immune': "But you're completely immune!",
  'monster_hit_resist': 'You resist, only %damage damage',
  'monster_hit_vuln': "It's SUPER BAD! %damage damage!",
  'monster_hit_crit': 'CRITICAL HIT! You take %damage damage!',
  'player_miss': 'But you miss!',
  'player_magic_miss': 'But it has no effect!',
  'player_hit': 'You deal %damage damage!',
  'player_hit_immune': "They're completely immune!",
  'player_hit_resist': "They resist your attack, only %damage damage...",
  'player_hit_vuln': "It's SUPER EFFECTIVE dealing %damage damage!",
  'player_hit_crit': 'CRITICAL HIT! You deal %damage damage!',
  'victory': 'Victory! You gain %exp XP!',
  'victory_no_xp': 'Victory! You gain no XP.\fMust have been too weak...',
  'level_up': 'LEVEL UP! You reach %level!',
})

/**
 * Summon replated strings.
 */
addNamespace('summon', 3, {
  // Summon 1 - Wilbur the Kobold
  'kobold_attack': 'You attack with your tiny claws...',
  'kobold_magic_attack': 'You fling a ball of acid...',
  'vine_whip': 'Vine Whip',
  'mend': ':regen:Mend',
  'summon_ants': ':defdown:Summon Ants',
  'dirty_fang': ':poison:Drity Fang',
  'dirty_fang_attack': 'You rush forward, fangs bared!',
  'dirty_fang_hit_poison': 'Your bite deals %damage damage and poison them!',
  'dirty_fang_hit_no_poison': 'Your filthy kobold bite deals %damage damage!',
  // Summon 2 - Beholder
  'beholder': 'Beholder',
});

/**
 * Monster specific battle strings
 */
addNamespace('monster', 3, {
  'kobold_axe': 'Kobold %c raises a tiny axe...',
  'kobold_fire': 'Kobold %c spits a glob of fire...',
  'kobold_dazed': 'Kobold %c looks dazed...',
  'kobold_does_nothing': 'And does nothing!',
  'kobold_miss': 'But instead it falls back and hiccups!',
})

// Export the namespaces
module.exports = namespaces;
