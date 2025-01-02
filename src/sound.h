#ifndef _SOUND_H
#define _SOUND_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

void sfx_test(void);

void sound_init(void);
void update_sound(void);

void sfx_stairs(void);
void sfx_error(void);
void sfx_wall_hit(void);
void sfx_menu_move(void);
void sfx_next_round(void);
void sfx_no_no_square(void);

#endif
