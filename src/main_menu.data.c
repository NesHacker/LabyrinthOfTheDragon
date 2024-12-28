#pragma bank 1

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include "main_menu.h"

/**
 * The main menu uses most of the 8 available palettes for specific graphics
 * elements on both the title screen, save select, and new game screens.
 *
 * - Palettes 0-3: These four palettes are used for core "background" colors
 *   for the title screen art, etc.
 * - Palettes 4-6: Save game select palettes. The colors for each of these are
 *   changed to denote that a particular save game is selected.
 */
const uint16_t main_menu_palettes[] = {
  // 0 - Background 1 "Textbox Text"
  RGB_BG_BLUE,
  RGB_SELECTED_BOX_BODY,
  RGB_SELECTED_BOX_BORDER,
  RGB_SELECTED_BOX_OUTLINE,
  // 1 - Background 2 "Free Text"
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_SELECTED_BOX_OUTLINE,
  // 2 - Background 3 (UNUSED)
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  // 3 - Background 4 (UNUSED)
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  // 4 - Save Select 1
  RGB_BG_BLUE,
  RGB_GRAY_LIGHT,
  RGB_GRAY_MID,
  RGB_GRAY_DARK,
  // 5 - Save Select 2
  RGB_BG_BLUE,
  RGB_GRAY_LIGHT,
  RGB_GRAY_MID,
  RGB_GRAY_DARK,
  // 6 - Save Select 3
  RGB_BG_BLUE,
  RGB_GRAY_LIGHT,
  RGB_GRAY_MID,
  RGB_GRAY_DARK,
  // 8 - UNUSED
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
  RGB_BG_BLUE,
};

const uint16_t save_1_selected_palettes[] = {
  PALETTE_SAVE_SELECTED,
  PALETTE_SAVE_DESELECTED,
  PALETTE_SAVE_DESELECTED,
};

const uint16_t save_2_selected_palettes[] = {
  PALETTE_SAVE_DESELECTED,
  PALETTE_SAVE_SELECTED,
  PALETTE_SAVE_DESELECTED,
};

const uint16_t save_3_selected_palettes[] = {
  PALETTE_SAVE_DESELECTED,
  PALETTE_SAVE_DESELECTED,
  PALETTE_SAVE_SELECTED,
};

const uint16_t save_none_selected_palettes[] = {
  PALETTE_SAVE_DESELECTED,
  PALETTE_SAVE_DESELECTED,
  PALETTE_SAVE_DESELECTED,
};

const uint16_t hero_palettes[] = {
  // Deselected Hero
  RGB(0, 0, 0),
  RGB_GRAY_LIGHT,
  RGB_GRAY_MID,
  RGB_GRAY_DARK,
  // Hero 1
  RGB(0, 0, 0),
  RGB8(245, 213, 135),
  RGB8(167, 75, 0),
  RGB8(8, 46, 54),
  // Hero 2
  RGB(0, 0, 0),
  RGB8(89, 60, 15),
  RGB8(131, 146, 32),
  RGB8(8, 46, 54),
  // Hero 3
  RGB(0, 0, 0),
  RGB8(200, 165, 45),
  RGB8(180, 69, 61),
  RGB8(61, 20, 97),
};
