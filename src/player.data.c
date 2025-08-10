#pragma bank 0

#include "player.h"
#include "strings.h"

const palette_color_t hero_colors[16] = {
  // Druid
  RGB_BLACK,
  RGB8(89, 60, 15),
  RGB8(131, 146, 31),
  RGB8(22, 27, 3),
  // Fighter
  RGB_BLACK,
  RGB8(245, 213, 135),
  RGB8(167, 75, 0),
  RGB8(8, 46, 54),
  // Monk
  RGB_BLACK,
  RGB8(109, 85, 28),
  RGB8(217, 204, 64),
  RGB8(33, 8, 41),
  // Sorcerer
  RGB_BLACK,
  RGB8(200, 165, 45),
  RGB8(144, 37, 49),
  RGB8(42, 14, 59),
};

//------------------------------------------------------------------------------

const Ability druid0 = {
  1, str_ability_druid_cure_wounds,
  TARGET_SELF, 4, druid_cure_wounds,
};

const Ability druid1 = {
  2, str_ability_druid_bark_skin,
  TARGET_SELF, 8, druid_bark_skin,
};

const Ability druid2 = {
  3, str_ability_druid_lightning,
  TARGET_SINGLE, 15, druid_lightning,
};

const Ability druid3 = {
   4, str_ability_druid_heal,
   TARGET_SELF, 19, druid_heal,
};

const Ability druid4 = {
   5, str_ability_druid_insect_plague,
   TARGET_ALL, 28, druid_insect_plague,
};

const Ability druid5 = {
   6, str_ability_druid_regen,
   TARGET_SELF, 33, druid_regen
};

//------------------------------------------------------------------------------

const Ability fighter0 = {
  1, str_ability_fighter_second_wind,
  TARGET_SELF, 7, fighter_second_wind,
};

const Ability fighter1 = {
  2, str_ability_fighter_action_surge,
  TARGET_SINGLE, 14, fighter_action_surge,
};

const Ability fighter2 = {
  3, str_ability_fighter_cleave,
  TARGET_ALL, 19, fighter_cleave,
};

const Ability fighter3 = {
  4, str_ability_fighter_trip_attack,
  TARGET_SINGLE, 23, fighter_trip_attack,
};

const Ability fighter4 = {
  5, str_ability_fighter_menace,
  TARGET_ALL, 28, fighter_menace,
};

const Ability fighter5 = {
  6, str_ability_fighter_indomitable,
  TARGET_SELF, 35, fighter_indomitable
};

//------------------------------------------------------------------------------

const Ability monk0 = {
  1, str_ability_monk_evasion,
  TARGET_SELF, 7, monk_evasion
};

const Ability monk1 = {
  2, str_ability_monk_open_palm,
  TARGET_SINGLE, 10, monk_open_palm
};

const Ability monk2 = {
  3, str_ability_monk_still_mind,
  TARGET_SELF, 13, monk_still_mind
};

const Ability monk3 = {
  4, str_ability_monk_flurry,
  TARGET_SINGLE, 19, monk_flurry
};

const Ability monk4 = {
  5, str_ability_monk_diamond_body,
  TARGET_SELF, 15, monk_diamond_body
};

const Ability monk5 = {
  6, str_ability_monk_quivering_palm,
  TARGET_SINGLE, 30, monk_quivering_palm
};

//------------------------------------------------------------------------------

const Ability sorcerer0 = {
  1, str_ability_sorc_darkness,
  TARGET_ALL, 4, sorcerer_darkness
};

const Ability sorcerer1 = {
  2, str_ability_sorc_fireball,
  TARGET_ALL, 12, sorcerer_fireball
};

const Ability sorcerer2 = {
  3, str_ability_sorc_haste,
  TARGET_SELF, 15, sorcerer_haste
};

const Ability sorcerer3 = {
  4, str_ability_sorc_sleetstorm,
  TARGET_ALL, 20, sorcerer_sleetstorm
};

const Ability sorcerer4 = {
  5, str_ability_sorc_disintegrate,
  TARGET_SINGLE, 28, sorcerer_disintegrate
};

const Ability sorcerer5 = {
  6, str_ability_sorc_wild_magic,
  TARGET_ALL, 33, sorcerer_wild_magic
};

//------------------------------------------------------------------------------

const Ability test_class0 = {
  1, "Damage All", TARGET_SELF, 0, test_class_ability0
};

const Ability test_class1 = {
  2, "(De)buff", TARGET_SELF, 0, test_class_ability1
};

const Ability test_class2 = {
  3, "SUPERKILL", TARGET_SELF, 0, test_class_ability2
};

const Ability test_class3 = {
  4, "Test 4", TARGET_SELF, 0, test_class_ability3
};

const Ability test_class4 = {
  5, "Test 5", TARGET_SELF, 0, test_class_ability4
};

const Ability test_class5 = {
  6, "Test 6", TARGET_SELF, 0, test_class_ability5
};
