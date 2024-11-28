#include "global.h"

/**
 * Global cursor position. A common variable used fro most menus in the game.
 */
uint8_t cursor = 0;

/**
 * Global walk timer. Used for various character walking animations.
 */
Timer walk_timer;

/**
 * Walk frame. Used for various walking animations.
 */
uint8_t walk_frame;
