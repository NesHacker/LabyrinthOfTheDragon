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
})

/**
 * In game summon name strings.
 */
addNamespace('summon_name', 0, {
  'beholder': 'Beholder',
  'kobold': 'Kobold',
  'wilbur': 'Wilbur',
})

/**
 * Ability names, attack strings, etc.
 */
addNamespace('ability', 0, {
  'vine_whip': 'Vine Whip',
  'mend': ':regen:Mend',
  'summon_ants': ':defdown:Summon Ants',
  'dirty_fang': ':poison:Drity Fang',
});

/**
 * Battle related strings.
 */
addNamespace('battle', 3, {
  'dirty_fang_attack': 'You rush forward, fangs bared!',
  'dirty_fang_hit_poison': 'Your bite deals %damage damage and poison them!',
  'dirty_fang_hit_no_poison': 'Your filthy kobold bite deals %damage damage!',
  'kobold_base_attack': 'You attack %monster %c with your claws...',
  'kobold_axe': 'Kobold %c attacks with a stone axe...',
  'monster_attack': ':fight: %monster %c attacks...',
  'monster_magic_attack': ':magic: %monster %c casts a spell...',
  'monster_miss': 'But they miss!',
  'monster_miss_magic': 'But the spell goes astray!',
  'monster_hit': 'They hit for %damage damage!',
  'monster_hit_aspect': 'They hit for %damage %aspect damage!',
  'player_miss': 'But you miss!',
  'player_magic_miss': 'But it has no effect!',
  'player_hit': 'You hit!',
  'player_hit_immune': "They're completely immune!",
  'player_hit_resist': "They resist your attack.",
  'player_hit_vuln': "It deals massive damage!",
})

// Export the namespaces
module.exports = namespaces;
