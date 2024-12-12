#ifndef _PALETTE_H
#define _PALETTE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdbool.h>

/**
 * Used to denote the type for an animated palette fade transition.
 */
typedef enum FadeType {
  FADE_STOPPED,
  FADE_OUT,
  FADE_IN
} FadeType;

/**
 * Initiates a fade-out transition.
 * @param c The color for the fade.
 */
void fade_out(void);

/**
 * Initiates a fade-in transition.
 */
void fade_in(void);

/**
 * Updates timers and states for fade transitions.
 * @return `true` if the fade animation is complete.
 */
bool fade_update(void);

/**
 * Holds the background palette color values currently loaded in VRAM. Changing
 * these values directly has no impact on the VRAM colors. To correctly update
 * palette data use `update_bg_palettes` or `update_fg_palettes`.
 */
extern palette_color_t bg_palettes[8 * 4];

/**
 * Holds the sprite palette color values currently loaded in VRAM. Changing
 * these values directly has no impact on the VRAM colors. To correctly update
 * palette data use `update_bg_palettes` or `update_fg_palettes`.
 */
extern palette_color_t sprite_palettes[8 * 4];

/**
 * Updates one or more background palettes.
 * @param i Starting index for the palettes to update.
 * @param n Number of palettes to update.
 * @param data Palette color data to use for the update.
 */
void update_bg_palettes(uint8_t i, uint8_t n, const palette_color_t *data);

/**
 * Updates one or more sprite palettes.
 * @param i Starting index for the palettes to update.
 * @param n Number of palettes to update.
 * @param data Palette color data to use for the update.
 */
void update_sprite_palettes(uint8_t i, uint8_t n, const palette_color_t *data);

#endif