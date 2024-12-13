#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

/**
 * State enumeration for the main menu.
 */
typedef enum MainMenuState {
  TITLE,
  SAVE_SELECT,
} MainMenuState;

extern MainMenuState main_menu_state;

void init_main_menu(void);
void update_main_menu(void);
void draw_main_menu(void);

// Cursor and sprite related constants
#define SAVE_SELECT_CURSOR_SAVE1 0
#define SAVE_SELECT_CURSOR_SAVE2 1
#define SAVE_SELECT_CURSOR_SAVE3 2
#define SAVE_SELECT_CURSOR_ERASE 3

#define SPRITE_CURSOR 0
#define SPRITE_HERO1  1
#define SPRITE_HERO2  5
#define SPRITE_HERO3  9

#define HERO1_X 32
#define HERO1_Y 48

#define HERO2_X 32
#define HERO2_Y 80

#define HERO3_X 32
#define HERO3_Y 112

// Colors
#define RGB_BG_BLUE               RGB8(139, 163, 207)
#define RGB_BG_BLUE_FADE1         RGB8(100, 120, 160)
#define RGB_BG_BLUE_FADE2         RGB8(60, 80, 120)
#define RGB_BG_BLUE_DARK          RGB8(20, 40, 80)

#define RGB_SELECTED_BOX_BORDER       RGB8(160, 105, 34)
#define RGB_SELECTED_BOX_BORDER_FADE1 RGB8(120, 80, 50)
#define RGB_SELECTED_BOX_BORDER_FADE2 RGB8(80, 60, 60)

#define RGB_SELECTED_BOX_BODY         RGB8(122, 147, 86)
#define RGB_SELECTED_BOX_BODY_FADE1   RGB8(80, 110, 80)
#define RGB_SELECTED_BOX_BODY_FADE2   RGB8(40, 80, 80)

#define RGB_SELECTED_BOX_OUTLINE        RGB8(45, 35, 21)
#define RGB_SELECTED_BOX_OUTLINE_FADE1  RGB8(35, 37, 41)
#define RGB_SELECTED_BOX_OUTLINE_FADE2  RGB8(25, 39, 61)

#define RGB_GRAY_DARK             RGB8(80, 80, 80)
#define RGB_GRAY_MID              RGB8(120, 120, 120)
#define RGB_GRAY_LIGHT            RGB8(180, 180, 180)

#define PALETTE_SAVE_SELECTED \
  RGB_BG_BLUE, \
  RGB_SELECTED_BOX_BORDER, \
  RGB_SELECTED_BOX_BODY, \
  RGB_SELECTED_BOX_OUTLINE \

#define PALETTE_SAVE_DESELECTED \
  RGB_BG_BLUE, \
  RGB_GRAY_LIGHT, \
  RGB_GRAY_MID, \
  RGB_GRAY_DARK

extern const uint16_t main_menu_palettes[];
extern const uint16_t save_1_selected_palettes[];
extern const uint16_t save_2_selected_palettes[];
extern const uint16_t save_3_selected_palettes[];
extern const uint16_t save_none_selected_palettes[];
extern const uint16_t hero_palettes[];

extern uint8_t cursor;

#endif
