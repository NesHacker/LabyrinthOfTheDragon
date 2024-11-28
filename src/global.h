#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "util.h"

/**
 * Global cursor position. A common variable used fro most menus in the game.
 */
extern uint8_t cursor;

/**
 * Global walk timer. Used for various character walking animations.
 */
extern Timer walk_timer;

/**
 * Walk frame. Used for various walking animations.
 */
extern uint8_t walk_frame;

#endif