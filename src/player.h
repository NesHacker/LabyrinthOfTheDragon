#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

typedef struct Monster {
  uint8_t id;
  char name[6];
  uint8_t level;
  uint8_t hp;
  uint8_t ability_ids[4];
} Monster;

typedef struct Player {
  uint8_t checksum;
  uint8_t is_empty;
  char name[6];
  uint8_t class_id;
  uint8_t hours_bcd[2];
  uint8_t minutes_bcd[2];
  uint8_t spirit_level;
  uint8_t monsters;
  uint8_t manual_entries;
  Monster monsters[64];
} HeroSave;

#endif
