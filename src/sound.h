#ifndef _SOUND_H
#define _SOUND_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

/**
 * Initializes the sound system.
 */
void sound_init(void) NONBANKED;

/**
 * Timer ISR that handles sound updates.
 */
void update_sound(void) NONBANKED;

/**
 * Plays the given sound.
 * @param sound Sound function to execute.
 */
void play_sound(void (*sound)(void)) NONBANKED;

void sfx_test(void);
void sfx_stairs(void);
void sfx_error(void);
void sfx_wall_hit(void);
void sfx_menu_move(void);
void sfx_next_round(void);
void sfx_no_no_square(void);
void sfx_melee_attack(void);
void sfx_monster_death(void);
void sfx_level_up(void);
void sfx_light_fire(void);
void sfx_open_chest(void);
void sfx_door_unlock(void);
void sfx_big_door_open(void);
void sfx_start_battle(void);
void sfx_monster_attack1(void);
void sfx_monster_attack2(void);
void sfx_monster_critical(void);
void sfx_monster_fail(void);
void sfx_battle_success(void);
void sfx_battle_death(void);
void sfx_action_surge(void);
void sfx_miss(void);
void sfx_heal(void);
void sfx_big_powerup(void);
void sfx_mid_powerup(void);
void sfx_poison_spray(void);
void sfx_monk_strike(void);
void sfx_evade(void);
void sfx_magic_missile(void);
void sfx_falling(void);

#endif
