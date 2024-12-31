#pragma bank 2

#include "core.h"
#include "textbox.h"

const Tilemap textbox_tilemap = { 20, 6, 1, tilemap_textbox };

const palette_color_t textbox_palette[4] = {
  RGB8(24, 75, 105),
  RGB8(24, 25, 105),
  RGB8(100, 100, 100),
  RGB8(200, 200, 200)
};

static void init_textbox(void) {
  core.load_bg_palette(textbox_palette, 7, 1);
  core.draw_tilemap(textbox_tilemap, VRAM_WINDOW);
  textbox.y = 144;
  textbox.state = TEXT_BOX_CLOSED;
  move_win(7, textbox.y);
  text_writer.set_origin(VRAM_WINDOW, 1, 1);
  text_writer.set_size(18, 4);
  text_writer.set_auto_page(AUTO_PAGE_OFF);
}

static void open_textbox(const char *text) {
  text_writer.clear();
  textbox.text = text;
  textbox.state = TEXT_BOX_OPENING;
}

static void update_textbox(void) {
  switch (textbox.state) {
  case TEXT_BOX_CLOSED:
    break;
  case TEXT_BOX_OPENING:
    if (textbox.y != 96) {
      textbox.y -= 2;
      move_win(7, textbox.y);
    } else {
      text_writer.print(textbox.text);
      textbox.state = TEXT_BOX_OPEN;
    }
    break;
  case TEXT_BOX_OPEN:
    text_writer.update();
    if (!was_pressed(J_A))
      return;
    if (text_writer.state == TEXT_WRITER_PAGE_WAIT)
      text_writer.next_page();
    else if (text_writer.state == TEXT_WRITER_DONE)
      textbox.state = TEXT_BOX_CLOSING;
    break;
  case TEXT_BOX_CLOSING:
    if (textbox.y < 144) {
      textbox.y += 2;
      move_win(7, textbox.y);
    } else {
      textbox.state = TEXT_BOX_CLOSED;
    }
    break;
  }
}

TextBox textbox = {
  init_textbox,
  open_textbox,
  update_textbox,
  TEXT_BOX_CLOSED,
  144
};

