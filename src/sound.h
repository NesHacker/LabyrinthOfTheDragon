#ifndef _SOUND_H
#define _SOUND_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

void sfx_test(void) BANKED;

void sound_init(void) BANKED;
void update_sound(void) BANKED;

void sfx_stairs(void) BANKED;
void sfx_error(void) BANKED;
void sfx_wall_hit(void) BANKED;
void sfx_menu_move(void) BANKED;
void sfx_next_round(void) BANKED;

#endif
