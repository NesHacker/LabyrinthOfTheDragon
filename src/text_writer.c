#include "text_writer.h"

static void text_writer_set_auto_page(AutoPageSpeed a) {
  text_writer.auto_page = a;
}

static void text_writer_set_size(uint8_t w, uint8_t h) {
  text_writer.width = w;
  text_writer.height = h;
}

static void text_writer_set_origin(uint8_t *vram, uint8_t col, uint8_t row) {
  text_writer.origin_column = col;
  text_writer.origin_row = row;
  text_writer.origin = vram + row * 32 + col;
}

static void text_writer_clear(void) {
  uint8_t *vram = text_writer.origin;

  VBK_REG = VBK_TILES;
  for (uint8_t row = 0; row < text_writer.height; row++) {
    uint8_t col;
    for (col = 0; col < text_writer.width; col++) {
      set_vram_byte(vram++, FONT_SPACE);
    }
    vram += VRAM_ROW_OFFSET(col);
  }

  text_writer.vram = text_writer.origin;
  text_writer.col = 0;
  text_writer.row = 0;
}

static void text_writer_print(const char *string) {
  text_writer.text = string;
  text_writer.state = TEXT_WRITER_START;
}

static void text_writer_next_page(void) {
  const char next = *text_writer.text;
  text_writer.state = TEXT_WRITER_CLEAR;
}

static void page_wait(void) {
  switch (text_writer.auto_page) {
  case AUTO_PAGE_OFF:
    text_writer.state = TEXT_WRITER_PAGE_WAIT;
    return;
  case AUTO_PAGE_SLOW:
    init_timer(text_writer.timer, AUTO_PAGE_SLOW_FRAMES);
    break;
  case AUTO_PAGE_MED:
    init_timer(text_writer.timer, AUTO_PAGE_MED_FRAMES);
    break;
  case AUTO_PAGE_FAST:
    init_timer(text_writer.timer, AUTO_PAGE_FAST_FRAMES);
    break;
  }
  text_writer.state = TEXT_WRITER_PAGE_DELAY;
}

static inline void text_end(void) {
  switch (text_writer.auto_page) {
  case AUTO_PAGE_OFF:
    text_writer.state = TEXT_WRITER_DONE;
    return;
  case AUTO_PAGE_SLOW:
    init_timer(text_writer.timer, AUTO_PAGE_SLOW_FRAMES);
    break;
  case AUTO_PAGE_MED:
    init_timer(text_writer.timer, AUTO_PAGE_MED_FRAMES);
    break;
  case AUTO_PAGE_FAST:
    init_timer(text_writer.timer, AUTO_PAGE_FAST_FRAMES);
    break;
  }
  text_writer.state = TEXT_WRITER_END_DELAY;
}

static void new_line(void) {
  if (text_writer.row == text_writer.height - 1) {
    page_wait();
    return;
  }
  const uint8_t offset = VRAM_ROW_OFFSET(text_writer.col + 1);
  text_writer.row++;
  text_writer.vram += offset + text_writer.origin_column;
  text_writer.col = 0;
}

static void text_writer_update(void) {
  switch (text_writer.state) {
  case TEXT_WRITER_DONE:
  case TEXT_WRITER_PAGE_WAIT:
    return;
  case TEXT_WRITER_START:
  case TEXT_WRITER_CLEAR:
    text_writer_clear();
    text_writer.state = TEXT_WRITER_WRITING;
    return;
  case TEXT_WRITER_WRITING:
    char next = *text_writer.text++;
    switch (next) {
    case '\0':
      text_end();
      return;
    case '\n':
      new_line();
      return;
    case '\f':
      page_wait();
      return;
    default:
      if (text_writer.col == text_writer.width) {
        new_line();
        return;
      }
      text_writer.col++;
      VBK_REG = VBK_TILES;
      set_vram_byte(text_writer.vram++, next + FONT_OFFSET);
      init_timer(text_writer.timer, text_writer.character_delay_frames);
      text_writer.state = TEXT_WRITER_CHAR_DELAY;
    }
    return;
  case TEXT_WRITER_CHAR_DELAY:
    if (update_timer(text_writer.timer))
      text_writer.state = TEXT_WRITER_WRITING;
    return;
  case TEXT_WRITER_PAGE_DELAY:
    if (update_timer(text_writer.timer))
      text_writer.state = TEXT_WRITER_CLEAR;
    return;
  case TEXT_WRITER_END_DELAY:
    if (update_timer(text_writer.timer))
      text_writer.state = TEXT_WRITER_DONE;
    return;
  default:
  }
}

TextWriter text_writer = {
  text_writer_set_auto_page,
  text_writer_set_size,
  text_writer_set_origin,
  text_writer_clear,
  text_writer_print,
  text_writer_next_page,
  text_writer_update,
  TEXT_WRITER_DONE,
  TEXT_WRITER_DEFAULT_WIDTH,
  TEXT_WRITER_DEFAULT_HEIGHT,
  AUTO_PAGE_OFF,
  TEXT_WRITER_DEFAULT_CHARACTER_DELAY,
};
