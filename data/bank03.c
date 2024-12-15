/**
 * [Core] Bank 3 - Battle System
 */

#pragma bank 3

#include <gb/gb.h>
#include <gb/cgb.h>
#include <gbdk/incbin.h>
#include <stdint.h>

const palette_color_t monster_death_colors[] = {
  // Palette 0
  RGB8(255, 255, 255),
  RGB8(207, 66, 66),
  RGB8(128, 41, 92),
  RGB8(42, 9, 69),
  // Palette 1
  RGB8(255, 255, 255),
  RGB8(207, 134, 134),
  RGB8(128, 41, 92),
  RGB8(42, 9, 69),
  // Palette 2
  RGB8(255, 255, 255),
  RGB8(207, 179, 179),
  RGB8(154, 95, 130),
  RGB8(42, 9, 69),
  // Palette 3
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(190, 144, 171),
  RGB8(82, 38, 119),
  // Palette 4
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(156, 123, 183),
  // Palette 5
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
  RGB8(255, 255, 255),
};
