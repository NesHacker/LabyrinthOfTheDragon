#pragma bank 0

#include "encounter.h"
#include "monster.h"

void generate_monster(
  Monster *m,
  MonsterType t,
  uint8_t l,
  PowerTier tier
) NONBANKED {
  switch (t) {
  case MONSTER_KOBOLD:
    kobold_generator(m, l, tier);
    break;
  case MONSTER_GOBLIN:
    goblin_generator(m, l, tier);
    break;
  case MONSTER_ZOMBIE:
    zombie_generator(m, l, tier);
    break;
  case MONSTER_BUGBEAR:
    bugbear_generator(m, l, tier);
    break;
  case MONSTER_OWLBEAR:
    owlbear_generator(m, l, tier);
    break;
  case MONSTER_GELATINOUS_CUBE:
    gelatinous_cube_generator(m, l, tier);
    break;
  case MONSTER_DISPLACER_BEAST:
    displacer_beast_generator(m, l, tier);
    break;
  case MONSTER_WILL_O_WISP:
    will_o_wisp_generator(m, l, tier);
    break;
  case MONSTER_DEATHKNIGHT:
    deathknight_generator(m, l, tier);
    break;
  case MONSTER_MINDFLAYER:
    mindflayer_generator(m, l, tier);
    break;
  case MONSTER_BEHOLDER:
    beholder_generator(m, l, tier);
    break;
  case MONSTER_DRAGON:
    dragon_generator(m, l, tier);
    break;
  }
}

void generate_encounter(EncounterTable *table) {
  uint8_t odds = 0;
  uint8_t roll = d256();

  while (table->odds != END) {
    odds += table->odds;
    *debug++ = odds;
    if (roll <= odds)
      break;
    table++;
  }

  *debug++ = 0xAA;
  *debug++ = table->odds;
  *debug++ = table->layout;

  reset_encounter(table->layout);
  Monster *monster = encounter.monsters;

  switch (table->layout) {
  case MONSTER_LAYOUT_1:
    generate_monster(
      monster, table->monster1, table->monster1_level, table->monster1_tier);
    monster->id = 'A';
    break;
  case MONSTER_LAYOUT_2:
    // Monster 1
    generate_monster(
      monster, table->monster1, table->monster1_level, table->monster1_tier);
    monster->id = 'A';
    monster++;

    // Monster 2
    generate_monster(
      monster, table->monster2, table->monster2_level, table->monster2_tier);
    if (table->monster1 == table->monster2)
      monster->id = 'B';
    else
      monster->id = 'A';
    break;
  default:
    // Monster 1
    generate_monster(
      monster, table->monster1, table->monster1_level, table->monster1_tier);
    monster->id = 'A';
    monster++;

    // Monster 2
    generate_monster(
      monster, table->monster2, table->monster2_level, table->monster2_tier);
    if (table->monster1 == table->monster2)
      monster->id = 'B';
    else
      monster->id = 'A';
    monster++;

    // Monster 3
    generate_monster(
      monster, table->monster3, table->monster3_level, table->monster3_tier);
    if (
      table->monster1 == table->monster2 &&
      table->monster1 == table->monster3
    ) {
      monster->id = 'C';
    } else if (
      table->monster1 == table->monster3 ||
      table->monster2 == table->monster3
    ) {
      monster->id = 'B';
    } else {
      monster->id = 'A';
    }
    break;
  }
}
