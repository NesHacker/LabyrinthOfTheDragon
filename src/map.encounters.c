#pragma bank 0

#include "encounter.h"
#include "monster.h"

static uint8_t steps = 0;
static uint8_t current_chance = 1;

static uint8_t safe_steps = 4;
static uint8_t chance_increase = 1;
static uint8_t initial_chance = 1;
static uint8_t torch_safe = true;

void config_random_encounter(uint8_t s, uint8_t ic, uint8_t i, bool ts) {
  safe_steps = s;
  initial_chance = ic;
  chance_increase = i;
  torch_safe = ts;
}

bool check_random_encounter(void) {
  MapTile *here = local_tiles;

  if (here->map_attr == MAP_EXIT)
    return false;

  if (torch_safe && player.torch_gauge > 0)
    return false;

  if (++steps <= safe_steps)
    return false;

  if (d64() > current_chance) {
    current_chance += chance_increase;
    return false;
  }

  steps = 0;
  current_chance = initial_chance;
  return true;
}

void generate_monster(
  Monster *monster,
  MonsterType type,
  uint8_t level,
  PowerTier tier
) NONBANKED {
  switch (type) {
  case MONSTER_KOBOLD:
    kobold_generator(monster, level, tier);
    break;
  case MONSTER_GOBLIN:
    goblin_generator(monster, level, tier);
    break;
  case MONSTER_ZOMBIE:
    zombie_generator(monster, level, tier);
    break;
  case MONSTER_BUGBEAR:
    bugbear_generator(monster, level, tier);
    break;
  case MONSTER_OWLBEAR:
    owlbear_generator(monster, level, tier);
    break;
  case MONSTER_GELATINOUS_CUBE:
    gelatinous_cube_generator(monster, level, tier);
    break;
  case MONSTER_DISPLACER_BEAST:
    displacer_beast_generator(monster, level, tier);
    break;
  case MONSTER_WILL_O_WISP:
    will_o_wisp_generator(monster, level, tier);
    break;
  case MONSTER_DEATHKNIGHT:
    deathknight_generator(monster, level, tier);
    break;
  case MONSTER_MINDFLAYER:
    mindflayer_generator(monster, level, tier);
    break;
  case MONSTER_BEHOLDER:
    beholder_generator(monster, level, tier);
    break;
  case MONSTER_DRAGON:
    dragon_generator(monster, level, tier);
    break;
  }
}

void generate_encounter(const EncounterTable *table) {
  uint16_t odds = 0;
  uint8_t roll = d256();

  while (table->odds != END) {
    odds += table->odds;
    if (roll <= odds)
      break;
    table++;
  }

  if (table->odds == END)
    table--;

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
