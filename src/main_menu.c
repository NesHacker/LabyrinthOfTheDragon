#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "joypad.h"
#include "main_menu.h"
#include "util.h"

/**
 * Palettes used by the main menu screen.
 */
const uint16_t main_menu_palettes[] = {
  // Palette 0
  RGB_BLACK,
  RGB8(80, 80, 80),
  RGB8(160, 160, 160),
  RGB_WHITE,
  // Palette 1 ("NES")
  RGB_BLACK,
  RGB8(80, 80, 80),
  RGB8(160, 160, 160),
  RGB8(196, 19, 39),
  // Palette 2 "HACKER"
  RGB_BLACK,
  RGB8(80, 80, 80),
  RGB8(160, 160, 160),
  RGB8(113, 113, 113),
  // Palette 3 (Book and Monster Sprite)
  RGB_BLACK,
  RGB8(160, 160, 160),
  RGB8(80, 80, 80),
  RGB_WHITE,
};


#define CURSOR_NEW_GAME 0
#define CURSOR_CONTINUE 1

/**
 * Position of the cursor on the main menu.
 */
uint8_t cursor = 0;

void init_main_menu(void) {
  scroll_bkg(0, 0);
  load_tilemap(tilemap_title_screen, 20, 18);
  set_bkg_palette(0, 4, main_menu_palettes);
  cursor = 0;
}

void update_main_menu(void) {
  uint8_t *top_vram = (void *)(0x9800 + 12 * 0x20 + 4);
  uint8_t *bottom_vram = (void *)(0x9800 + 14 * 0x20 + 4);

  if (was_pressed(J_UP | J_DOWN)) {
    VBK_REG = VBK_TILES;
    if (cursor == 0) {
      set_vram_byte(top_vram, 0x00);
      set_vram_byte(bottom_vram, 0xFE);
    } else {
      set_vram_byte(top_vram, 0xFE);
      set_vram_byte(bottom_vram, 0x00);
    }
    cursor ^= 1; 
  }
}
