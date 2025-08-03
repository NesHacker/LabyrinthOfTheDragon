#ifndef _HERO_SELECT_H
#define _HERO_SELECT_H

#include <gb/gb.h>

/**
 * Initializes the hero select screen.
 */
void init_hero_select(void) NONBANKED;

/**
 * Performs game loop updates for the hero select screen.
 */
void update_hero_select(void) NONBANKED;

#endif
