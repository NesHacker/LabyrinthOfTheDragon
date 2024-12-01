#ifndef _HERO_H
#define _HERO_H

#include "util.h"

typedef enum HeroState {
  HERO_STILL,
  HERO_WALKING,
} HeroState;

extern HeroState hero_state;
extern Direction hero_direction;
extern uint8_t hero_x;
extern uint8_t hero_y;

/**
 * Initializes the hero character's state and sprites.
 */
void init_hero(void);

/**
 * Updates and animates the sprites for the hero character.
 */
void update_hero(void);

/**
 * Clears sprites and data for the hero character.
 */
void clear_hero(void);

#endif
