#include "palette.h"

FadeType fade_type;
uint8_t fade_step = 0;

palette_color_t bg_palettes[8 * 4] = {
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
};

palette_color_t sprite_palettes[8 * 4] = {
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
};

palette_color_t fade_palettes[32] = {
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
  RGB_WHITE, RGB_WHITE, RGB_WHITE, RGB_WHITE,
};

void update_bg_palettes(uint8_t i, uint8_t n, const palette_color_t *data) {
  set_bkg_palette(i, n, data);
  palette_color_t *dst = bg_palettes + i * 4;
  for (uint8_t k = 0; k < 4 * n; k++) {
    *dst++ = *data++;
  }
}

void update_sprite_palettes(uint8_t i, uint8_t n, const palette_color_t *data) {
  set_sprite_palette(i, n, data);
  palette_color_t *dst = sprite_palettes + (i << 2);
  for (uint8_t k = 0; k < 4 * n; k++) {
    *dst++ = *data++;
  }
}

void fade_out(void) {
  fade_step = 16;
  fade_type = FADE_OUT;
  palette_color_t *dst = fade_palettes;
  palette_color_t *src = bg_palettes;
  for (uint8_t k = 0; k < 4 * 8; k++)
    *dst++ = *src++;
}

void fade_in(void) {
  fade_step = 16;
  fade_type = FADE_IN;
}

static void update_fade_out(uint8_t k) {
  palette_color_t target = RGB_WHITE;
  palette_color_t color = fade_palettes[k];
  if (color == target) return;

  uint16_t r = color & 0x1F;
  uint16_t g = (color >> 5) & 0x1F;
  uint16_t b = color >> 10;

  uint16_t tr = target & 0x1F;
  uint16_t tg = (target >> 5) & 0x1F;
  uint16_t tb = target >> 10;

  if (r < tr) {
    r += 2;
    if (r > tr) r = tr;
  }

  if (g < tg) {
    g += 2;
    if (g > tg) g = tg;
  }

  if (b < tb) {
    b += 2;
    if (b > tb) b = tb;
  }

  fade_palettes[k] = RGB(r, g, b);
}

static void update_fade_in(uint8_t k) {
  palette_color_t target = bg_palettes[k];
  palette_color_t color = fade_palettes[k];
  if (color == target) return;

  uint16_t r = color & 0x1F;
  uint16_t g = (color >> 5) & 0x1F;
  uint16_t b = color >> 10;

  uint16_t tr = target & 0x1F;
  uint16_t tg = (target >> 5) & 0x1F;
  uint16_t tb = target >> 10;

  if (r > tr) {
    if (r == 1) {
      r = 0;
    } else {
      r -= 2;
      if (r < tr) r = tr;
    }
  }

  if (g > tg) {
    if (g == 1) {
      g = 0;
    } else {
      g -= 2;
      if (g < tg) g = tg;
    }
  }

  if (b > tb) {
    if (b == 1) {
      b = 0;
    } else {
      b -= 2;
      if (b < tb) b = tb;
    }
  }

  fade_palettes[k] = RGB(r, g, b);
}

bool fade_update(void) {
  if (fade_type == FADE_STOPPED)
    return true;

  if (fade_type == FADE_OUT) {
    for (uint8_t k = 0; k < 32; k++) {
      update_fade_out(k);
    }
  } else {
    for (uint8_t k = 0; k < 32; k++) {
      update_fade_in(k);
    }
  }

  set_bkg_palette(0, 8, fade_palettes);

  if (--fade_step == 0) {
    fade_type = FADE_STOPPED;
    return true;
  }

  return false;
}
