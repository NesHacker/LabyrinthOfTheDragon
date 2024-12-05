#ifndef _MONSTER_H
#define _MONSTER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct MonsterMove {
  uint8_t id;
  char name[13];
  char type[8];
  uint8_t action_points;
  uint8_t max_action_points;
} MonsterMove;

typedef struct Monster {
  uint8_t id;
  char name[8];
  uint8_t attack;
  uint8_t armor_class;
  uint8_t magic_attack;
  uint8_t saving_throw;
  uint8_t initiative;
  MonsterMove skills[4];
  uint8_t skill_points[4];
  // tileset bank/addr
} Monster;

#endif
