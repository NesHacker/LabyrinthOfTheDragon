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
  TARGET_SELF, 12, druid_bark_skin,
};

const Ability druid2 = {
  3, str_ability_druid_ligtning,
  TARGET_SINGLE, 18, druid_lightning,
};

const Ability druid3 = {
   4, str_ability_druid_heal,
   TARGET_SELF, 20, druid_heal,
};

const Ability druid4 = {
   5, str_ability_druid_insect_plague,
   TARGET_ALL, 45, druid_insect_plague,
};

const Ability druid5 = {
   6, str_ability_druid_regen,
   TARGET_SELF, 30, druid_regen
};

//------------------------------------------------------------------------------

const Ability fighter0 = { 1 };
const Ability fighter1 = { 2 };
const Ability fighter2 = { 3 };
const Ability fighter3 = { 4 };
const Ability fighter4 = { 5 };
const Ability fighter5 = { 6 };

//------------------------------------------------------------------------------

const Ability monk0 = { 1 };
const Ability monk1 = { 2 };
const Ability monk2 = { 3 };
const Ability monk3 = { 4 };
const Ability monk4 = { 5 };
const Ability monk5 = { 6 };

//------------------------------------------------------------------------------

const Ability sorcerer0 = { 1 };
const Ability sorcerer1 = { 2 };
const Ability sorcerer2 = { 3 };
const Ability sorcerer3 = { 4 };
const Ability sorcerer4 = { 5 };
const Ability sorcerer5 = { 6 };

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
