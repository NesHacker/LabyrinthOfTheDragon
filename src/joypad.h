/**
 * Common joypad state and macros.
 */

#ifndef _JOYPAD_H
#define _JOYPAD_H

#include <gb/gb.h>

/**
 * Bitfield for buttons that are currently being held down.
 *
 * @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
 */
extern uint8_t joypad_down;

/**
 * Bitfield for buttons that were pressed as of this frame.
 *
 * @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
 */
extern uint8_t joypad_pressed;

/**
 * Bitfield for buttons that were released as of this frame.
 *
 * @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
 */
extern uint8_t joypad_released;

/**
 * @param b Button bitmask.
 * @return `true` If the button is down.
 */
#define is_down(b)      (joypad_down & (b))

/**
 * @param b Button bitmask.
 * @return `true` If the button was pressed this frame.
 */
#define was_pressed(b)  (joypad_pressed & (b))

/**
 * @param b Button bitmask.
 * @return `true` If the button was released this frame.
 */
#define was_released(b) (joypad_released & (b))

#endif