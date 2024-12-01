#ifndef _HERO_H
#define _HERO_H

typedef enum HeroState {
  HERO_STILL,
  HERO_WALKING,
} HeroState;

typedef enum Direction { DOWN, UP, LEFT, RIGHT } Direction;

extern HeroState hero_state;
extern Direction hero_direction;

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
