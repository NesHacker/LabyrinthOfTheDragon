#ifndef _BATTLE_H
#define _BATTLE_H

/**
 * Initializes the battle system.
 */
void init_battle(void);

/**
 * Game loop update for the battle system.
 */
void update_battle(void);

/**
 * VBLANK draw updates for the battle system.
 */
void draw_battle(void);

#endif
