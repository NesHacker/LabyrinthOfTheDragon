#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include <gb/cgb.h>

typedef enum TextboxState {
  TEXTBOX_CLOSED,
  TEXTBOX_OPENING,
  TEXTBOX_CLOSING,
  TEXTBOX_RESET,
  TEXTBOX_CLEAR,
  TEXTBOX_CLEAR_WAIT,
  TEXTBOX_PRINTING,
  TEXTBOX_LINE_WAIT,
  TEXTBOX_SCROLL,
  TEXTBOX_PAGE_WAIT,
  TEXTBOX_FINISHED
} TextboxState;

#define TILEMAP_TEXTBOX_W 20
#define TILEMAP_TEXTBOX_H 6

#define TEXTBOX_CLEAR_DELAY     10
#define TEXTBOX_CHAR_DELAY      3
#define TEXTBOX_SCROLL_DELAY    6
#define TEXTBOX_ARROW_DELAY     40
#define TEXTBOX_MAX_LINE_CHARS  17

#define TEXTBOX_PAGE_BREAK  '\f'
#define TEXTBOX_LINE_BREAK  '\n'
#define TEXTBOX_END         0

extern TextboxState textbox_state;
extern palette_color_t textbox_palette[4];

void init_text_box(void);
void open_textbox(const char *text);
void update_textbox(void);
void clear_textbox(void);

#endif
