#pragma bank 1

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "core.h"
#include "main_menu.h"
#include "sound.h"

#define MAX_ANIMATION_SPRITES 20

void init_dragon_eyes(void);
void update_dragon_eyes(void);

void init_neshacker_presents(void);
void update_neshacker_presents(void);

void init_fire_animation(void);
void update_fire_animation(void);

void init_smoke_animation(void);
void stop_smoke_animation(void);
void update_smoke_animation(void);


/**
 * Clears all sprites used by a title screen animation.
 */
static void clear_sprites(void) {
  for (uint8_t k = 0; k < MAX_ANIMATION_SPRITES; k++) {
    set_sprite_tile(k, 0);
    set_sprite_prop(k, 0);
    move_sprite(k, 0, 0);
  }
}

//------------------------------------------------------------------------------
// Title Screen Main State Controller
//------------------------------------------------------------------------------

typedef enum TitleState {
  TITLE_NESHACKER_PRESENTS,
  TITLE_DRAGON_FIRE,
  TITLE_PRESS_START,
} TitleState;


const palette_color_t default_palette[] = {
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
};

void init_title_screen(void) {
  DISPLAY_OFF;
  scroll_bkg(0, 0);

  core.load_title_tiles();
  core.load_bg_palette(default_palette, 0, 8);
  core.load_sprite_palette(default_palette, 0, 8);

  core.fill_bg(0xFF, 0b00001010);
  uint8_t *vram = VRAM_BACKGROUND_XY(4, 8);

  // Load tilesets and palettes
  // core.load_bg_palette(palettes, 0, 6);


  // core.load_sprite_palette(fg_palettes, 0, 2);

  SWITCH_ROM(1);

  // init_neshacker_presents();

  init_dragon_eyes();


  // init_fire_animation();
  // init_smoke_animation();

  // core.draw_tilemap(title_screen_tilemap, VRAM_BACKGROUND);

  DISPLAY_ON;
}

void update_title_screen(void) {
  // update_fire_animation();
  // update_smoke_animation();
  // update_neshacker_presents();
  update_dragon_eyes();
}

//------------------------------------------------------------------------------
// NESHACKER Presents
//------------------------------------------------------------------------------

static const palette_color_t neshacker_palette[] = {
  // FRAME 1
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,

  // FRAME 2
  RGB_BLACK, RGB8(40, 0, 0), RGB_BLACK, RGB8(80, 0, 0), // nes
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB8(20, 20, 20), // hacker

  // FRAME 3
  RGB_BLACK, RGB8(60, 0, 0), RGB8(20, 0, 0), RGB8(100, 0, 0), // nes
  RGB_BLACK, RGB8(20, 20, 20), RGB_BLACK, RGB8(40, 40, 40), // hacker

  // FRAME 4
  RGB_BLACK, RGB8(80, 0, 0), RGB8(40, 0, 0), RGB8(130, 0, 0), // nes
  RGB_BLACK, RGB8(40, 40, 40), RGB8(20, 20, 20), RGB8(60, 60, 60), // hacker

  // FRAME 5
  RGB_BLACK, RGB8(105, 5, 10), RGB8(64, 2, 4), RGB8(193, 18, 28), // nes
  RGB_BLACK, RGB8(68, 68, 68), RGB8(43, 43, 43), RGB8(85, 85, 85), // hacker
};

static const palette_color_t presents_palette[] = {
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK,           // Frame 1
  RGB_BLACK, RGB8(24, 24, 24), RGB_BLACK, RGB_BLACK,    // Frame 2
  RGB_BLACK, RGB8(50, 50, 50), RGB_BLACK, RGB_BLACK,    // Frame 3
  RGB_BLACK, RGB8(90, 90, 90), RGB_BLACK, RGB_BLACK,    // Frame 4
  RGB_BLACK, RGB8(118, 118, 118), RGB_BLACK, RGB_BLACK, // Frame 5
};

typedef enum NesHackerPresentsState {
  NHP_PRE_DELAY,
  NHP_FADE_IN,
  NHP_PRESENTS_DELAY,
  NHP_PRESENTS_FADE_IN,
  NHP_HOLD,
  NHP_FADE_OUT,
  NHP_DONE,
} NesHackerPresentsState;

NesHackerPresentsState nhp_state = NHP_FADE_IN;
Timer nhp_timer;
uint8_t neshacker_palette_idx;
Tilemap neshacker_presents_tilemap = { 12, 3, 1, tilemap_neshacker_presents };


void init_neshacker_presents(void) {
  // Draw the tilemap
  core.fill_bg(0xFF, 0b00001010);
  uint8_t *vram = VRAM_BACKGROUND_XY(4, 8);
  core.draw_tilemap(neshacker_presents_tilemap, vram);

  // Set the initial animation palettes
  core.load_bg_palette(neshacker_palette, 0, 2);
  core.load_bg_palette(presents_palette, 2, 1);

  // Initialize core state
  nhp_state = NHP_PRE_DELAY;
  init_timer(nhp_timer, 10);
}

void update_neshacker_presents(void) {
  switch (nhp_state) {
  case NHP_PRE_DELAY:
    if (!update_timer(nhp_timer))
      return;
    play_sound(sfx_neshacker_presents);
    nhp_state = NHP_FADE_IN;
    neshacker_palette_idx = 0;
    init_timer(nhp_timer, 3);
    break;
  case NHP_FADE_IN:
    if (!update_timer(nhp_timer))
      return;

    neshacker_palette_idx++;

    if (neshacker_palette_idx > 4) {
      init_timer(nhp_timer, 1);
      nhp_state = NHP_PRESENTS_DELAY;
      return;
    }

    reset_timer(nhp_timer);
    core.load_bg_palette(neshacker_palette + neshacker_palette_idx * 8, 0, 2);

    break;
  case NHP_PRESENTS_DELAY:
    if (!update_timer(nhp_timer))
      return;

    core.load_bg_palette(presents_palette + 4, 2, 1);

    nhp_state = NHP_PRESENTS_FADE_IN;
    neshacker_palette_idx = 0;
    init_timer(nhp_timer, 3);
    break;
  case NHP_PRESENTS_FADE_IN:
    if (!update_timer(nhp_timer))
      return;

    neshacker_palette_idx++;

    if (neshacker_palette_idx > 4) {
      nhp_state = NHP_HOLD;
      init_timer(nhp_timer, 60);
      return;
    }

    reset_timer(nhp_timer);
    core.load_bg_palette(presents_palette + 4 * neshacker_palette_idx, 2, 1);

    break;
  case NHP_HOLD:
    if (!update_timer(nhp_timer))
      return;
    nhp_state = NHP_FADE_OUT;
    neshacker_palette_idx = 5;
    init_timer(nhp_timer, 3);
    break;
  case NHP_FADE_OUT:
    if (!update_timer(nhp_timer))
      return;

    neshacker_palette_idx--;

    if (neshacker_palette_idx == 0xFF) {
      // TODO Transition to splash screen
      nhp_state = NHP_DONE;
      return;
    }

    reset_timer(nhp_timer);
    core.load_bg_palette(neshacker_palette + 8 * neshacker_palette_idx, 0, 2);
    core.load_bg_palette(presents_palette + 4 * neshacker_palette_idx, 2, 1);

    break;
  }
}

//------------------------------------------------------------------------------
// Dragon Eyes
//------------------------------------------------------------------------------

typedef enum DragonEyesState {
  DE_PRE_DELAY,
  DE_FADE_IN,
  DE_SMILE_DELAY,
  DE_FADE_OUT_DELAY,
  DE_FADE_OUT,
  DE_DONE,
} DragonEyesState;

const uint8_t eyes_x[] = {
  64,         88,
  64, 72, 80, 88,
      72, 80,
};

const uint8_t eyes_y[] = {
   88,            88,
  104, 104, 104, 104,
       112, 112,
};

const palette_color_t eyes_palette[] = {
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB8(0, 0, 0), // Frame 1
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB8(50, 0, 0), // Frame 2
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB8(100, 36, 0), // Frame 3
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB8(160, 110, 0), // Frame 4
  RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB8(252, 216, 0), // Frame 5
};

DragonEyesState de_state = DE_DONE;
uint8_t de_palette_idx = 0;
Timer de_timer;

void init_dragon_eyes(void) {
  core.load_sprite_palette(eyes_palette, 0, 1);

  for (uint8_t k = 0; k < 8; k++) {
    set_sprite_tile(k, 0xE0 + k);
    set_sprite_prop(k, 0x00);
    move_sprite(k, eyes_x[k] + 8, eyes_y[k]);
  }

  de_state = DE_PRE_DELAY;
  de_palette_idx = 0;
  init_timer(de_timer, 5);
}

void update_dragon_eyes(void) {
  switch (de_state) {
  case DE_PRE_DELAY:
    if (!update_timer(de_timer))
      return;
    init_timer(de_timer, 25);
    de_state = DE_FADE_IN;
    break;
  case DE_FADE_IN:
    if (!update_timer(de_timer))
      return;

    de_palette_idx++;
    if (de_palette_idx == 5) {
      init_timer(de_timer, 45);
      de_state = DE_SMILE_DELAY;
      return;
    }

    reset_timer(de_timer);
    core.load_sprite_palette(eyes_palette + de_palette_idx * 4, 0, 1);

    break;
  case DE_SMILE_DELAY:
    if (!update_timer(de_timer))
      return;

    for (uint8_t k = 0; k < 8; k++)
      set_sprite_tile(k, 0xE8 + k);

    de_state = DE_FADE_OUT_DELAY;
    init_timer(de_timer, 15);

    break;
  case DE_FADE_OUT_DELAY:
    if (!update_timer(de_timer))
      return;
    de_palette_idx = 4;
    init_timer(de_timer, 6);
    de_state = DE_FADE_OUT;
    break;
  case DE_FADE_OUT:
    if (!update_timer(de_timer))
      return;

    de_palette_idx--;
    reset_timer(de_timer);
    core.load_sprite_palette(eyes_palette + de_palette_idx * 4, 0, 1);

    if (de_palette_idx == 0) {
      init_timer(de_timer, 45);
      de_state = DE_DONE;
      return;
    }

    break;
  case DE_DONE:
    break;
  }
}

//------------------------------------------------------------------------------
// Main Title (Dragon Flame, Smoke, Press Start Animations w/ Joypad Input)
//------------------------------------------------------------------------------

static const palette_color_t fg_palettes[] = {
  // 0 - Fire
  RGB_BLACK,
  RGB8(252, 216, 0),
  RGB8(252, 108, 0),
  RGB_WHITE,
  // 1 - Smoke
  RGB_BLACK,
  RGB8(91, 91, 91),
  RGB8(156, 156, 156),
  RGB_WHITE,
};

static const palette_color_t dragon_palette[] = {
   // Palette 1 - Title
  RGB8(251, 242, 54),
  RGB8(48, 96, 130),
  RGB8(172, 50, 50),
  RGB_BLACK,
  // Palette 2 - Face
  RGB8(252, 216, 0),
  RGB8(154, 32, 24),
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  // Palette 3 - Wings
  RGB8(154, 32, 24),
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  RGB_BLACK,
  // Palette 4 - Outer Body
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  RGB8(9, 4, 4),
  RGB_BLACK,
  // Palette 5 - Head
  RGB8(154, 32, 24),
  RGB8(54, 11, 13),
  RGB8(25, 8, 10),
  RGB_BLACK,
  // Palette 6 - PRESS START
  // RGB8(251, 242, 54),
  RGB8(200, 0, 200),
  RGB_BLACK,
  RGB_BLACK,
  RGB_BLACK,
};

//------------------------------------------------------------------------------
// Fire Animation
//------------------------------------------------------------------------------

#define FIRE_FRAME_SPRITE_0 0
#define FIRE_FRAME_SPRITES 20

Timer fire_timer;
uint8_t fire_frame_idx = 0;

const uint8_t fire_animation_tiles[] = {
  // Frame 1
                    0x00, 0x01,
                    0x02, 0x03,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  // Frame 2
                    0x08, 0x09,
                    0x0A, 0x0B,
  0x07, 0x07, 0x07, 0x0C, 0x0D, 0x07, 0x07, 0x07,
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  // Frame 3
                    0x10, 0x11,
                    0x12, 0x13,
  0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
  0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,
  // Frame 4
                    0x24, 0x25,
                    0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  // Frame 5
                    0x38, 0x39,
                    0x3A, 0x3B,
  0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43,
  0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B,
};

const uint8_t fire_sprite_x[FIRE_FRAME_SPRITES] = {
  72, 80,
  72, 80,
  48, 56, 64, 72, 80, 88, 96, 104,
  48, 56, 64, 72, 80, 88, 96, 104,
};

const uint8_t fire_sprite_y[FIRE_FRAME_SPRITES] = {
  112, 112,
  120, 120,
  128, 128, 128, 128, 128, 128, 128, 128,
  136, 136, 136, 136, 136, 136, 136, 136,
};

const uint8_t fire_frames[] = {
  0, 1,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  2, 3, 4, 2, 3, 4, 2, 3, 4,
  END
};

void init_fire_animation(void) {
  init_timer(fire_timer, 6);
  fire_frame_idx = 0;

  for (uint8_t k = 0; k < FIRE_FRAME_SPRITES; k++) {
    const uint8_t id = FIRE_FRAME_SPRITE_0 + k;
    set_sprite_tile(id, fire_animation_tiles[k]);
    set_sprite_prop(id, 0x08);
    move_sprite(id, fire_sprite_x[k] + 8, fire_sprite_y[k] + 16);
  }
}

void update_fire_animation(void) {
  if (fire_frame_idx == END)
    return;

  if (!update_timer(fire_timer))
    return;
  reset_timer(fire_timer);
  fire_frame_idx++;

  uint8_t fire_frame_offset = fire_frames[fire_frame_idx];
  if (fire_frame_offset == END) {
    fire_frame_idx = END;
    clear_sprites();
    return;
  }

  for (uint8_t k = 0; k < FIRE_FRAME_SPRITES; k++) {
    const uint8_t tile_offset = fire_frame_offset * FIRE_FRAME_SPRITES;
    set_sprite_tile(FIRE_FRAME_SPRITE_0 + k, fire_animation_tiles[tile_offset + k]);
  }
}

//------------------------------------------------------------------------------
// Smoke Animation
//------------------------------------------------------------------------------

#define SMOKE_FRAMES 6
#define SMOKE_SPRITE_COUNT 18
#define SMOKE_TILE_OFFSET 0x50

Tilemap title_screen_tilemap = { 20, 18, 1, tilemap_title_screen };

const uint8_t smoke_tiles[] = {
  // Frame 1
  0x29, 0x29, 0x00, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
  0x00, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,

  // Frame 2
  0x29, 0x01, 0x02, 0x29, 0x03, 0x04, 0x29, 0x29, 0x29,
  0x02, 0x01, 0x29, 0x04, 0x03, 0x29, 0x29, 0x29, 0x29,

  // Frame 3
  0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
  0x07, 0x06, 0x05, 0x0A, 0x09, 0x08, 0x0D, 0x0C, 0x0B,

  // Frame 4
  0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
  0x10, 0x0F, 0x0E, 0x13, 0x12, 0x11, 0x16, 0x15, 0x14,

  // Frame 5
  0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x19, 0x18, 0x17, 0x1C, 0x1B, 0x1A, 0x1F, 0x1E, 0x1D,

  // Frame 6
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
  0x21, 0x21, 0x20, 0x25, 0x24, 0x23, 0x28, 0x27, 0x26
};

typedef enum SmokeAnimationState {
  SMOKE_ANIMATION_SMOKE,
  SMOKE_ANIMATION_PAUSE,
} SmokeAnimationState;

Timer smoke_timer;
uint8_t smoke_timer_idx = 0;
SmokeAnimationState smoke_animation_state;

const uint8_t smoke_sprite_x[] = {
  0, 8, 16, 0, 8, 16, 0, 8, 16,
  24, 32, 40, 24, 32, 40, 24, 32, 40,
};
const uint8_t smoke_sprite_y[] = {
  0, 0, 0, 8, 8, 8, 16, 16, 16,
  0, 0, 0, 8, 8, 8, 16, 16, 16,
};

#define SMOKE_OFFSET_X 7 * 8 - 6
#define SMOKE_OFFSET_Y 12 * 8 + 6


void stop_smoke_animation(void) {
  smoke_timer_idx = END;
}

void init_smoke_animation(void) {
  init_timer(smoke_timer, 6);
  smoke_animation_state = SMOKE_ANIMATION_SMOKE;
  smoke_timer_idx = 0;

  for (uint8_t k = 0; k < SMOKE_SPRITE_COUNT; k++) {
    set_sprite_tile(k, smoke_tiles[k] + 0x50);
    if (k >= 9)
      set_sprite_prop(k, 0x29);
    else
      set_sprite_prop(k, 0x09);

    uint8_t x_offset = k >= 9 ? 11 : 0;
    uint8_t y_offset = k >= 9 ? 1 : 0;

    move_sprite(k,
      smoke_sprite_x[k] + 8 + SMOKE_OFFSET_X + x_offset,
      smoke_sprite_y[k] + 16 + SMOKE_OFFSET_Y + y_offset);
  }
}

void update_smoke_animation(void) {
  if (smoke_timer_idx == END)
    return;

  switch(smoke_animation_state) {
  case SMOKE_ANIMATION_SMOKE:
    if (!update_timer(smoke_timer))
      return;
    reset_timer(smoke_timer);

    smoke_timer_idx++;

    if (smoke_timer_idx >= 6) {
      init_timer(smoke_timer, 100);
      smoke_animation_state = SMOKE_ANIMATION_PAUSE;
      for (uint8_t k = 0; k < SMOKE_SPRITE_COUNT; k++) {
        set_sprite_tile(k, 0x29 + 0x50);
      }
      return;
    }

    uint8_t frame_offset = smoke_timer_idx * 18;
    for (uint8_t k = 0; k < SMOKE_SPRITE_COUNT; k++) {
      set_sprite_tile(k, smoke_tiles[k + frame_offset] + 0x50);
    }

    break;
  case SMOKE_ANIMATION_PAUSE:
    if (!update_timer(smoke_timer))
      return;
      init_smoke_animation();
    break;
  }
}