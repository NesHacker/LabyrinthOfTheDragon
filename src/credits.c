#pragma bank 1

#include <gb/gb.h>
#include <gb/cgb.h>

#include "core.h"
#include "credits.h"
#include "palette.h"
#include "sound.h"
#include "strings.h"
#include "title_screen.h"

typedef enum CreditsState {
  CREDITS_INIT,
  CREDITS_FADE_IN,
  CREDITS_HOLD,
  CREDITS_FADE_OUT,
  CREDITS_INIT_SCROLL,
  CREDITS_SCROLL,
  CREDITS_FIN,
} CreditsState;

CreditsState credits_state = CREDITS_INIT;

typedef enum TextPage {
  CREDIT_TEXT_STORY1 = 0,
  CREDIT_TEXT_STORY2 = 1,
  CREDIT_TEXT_STORY3 = 2,
  CREDIT_TEXT_DEVELOPED_BY = 3,
  CREDIT_TEXT_PROG_DESIGN = 4,
  CREDIT_TEXT_ART = 5,
  CREDIT_TEXT_PATREON = 6,
  CREDIT_TEXT_THANK_YOU = 7,
} TextPage;

palette_color_t credits_palette[] = {
  RGB_WHITE,
  RGB_WHITE,
  RGB_DARKGRAY,
  RGB_BLACK,
};

TextPage credits_text_page;
Timer credits_timer;

void next_text_page(void) {
  core.fill_bg(0xA0, 0x08);

  switch (credits_text_page) {
  case CREDIT_TEXT_STORY1:
    core.draw_text(VRAM_BACKGROUND_XY(1, 7), str_credits_story1_1, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 8), str_credits_story1_2, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 9), str_credits_story1_3, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 10), str_credits_story1_4, 21);
    break;
  case CREDIT_TEXT_STORY2:
    core.draw_text(VRAM_BACKGROUND_XY(1, 7), str_credits_story2_1, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 8), str_credits_story2_2, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 9), str_credits_story2_3, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 10), str_credits_story2_4, 21);
    break;
  case CREDIT_TEXT_STORY3:
    core.draw_text(VRAM_BACKGROUND_XY(1, 7), str_credits_story3_1, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 8), str_credits_story3_2, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 9), str_credits_story3_3, 21);
    core.draw_text(VRAM_BACKGROUND_XY(1, 10), str_credits_story3_4, 21);
    break;
  case CREDIT_TEXT_DEVELOPED_BY:
    core.draw_text(VRAM_BACKGROUND_XY(4, 7), str_credits_developed_by, 20);
    core.draw_text(VRAM_BACKGROUND_XY(4, 8), str_credits_neshacker, 20);
    break;
  case CREDIT_TEXT_PROG_DESIGN:
    core.draw_text(VRAM_BACKGROUND_XY(3, 6), str_credits_programming, 20);
    core.draw_text(VRAM_BACKGROUND_XY(3, 7), str_credits_ryan, 20);

    core.draw_text(VRAM_BACKGROUND_XY(3, 9), str_credits_game_design, 20);
    core.draw_text(VRAM_BACKGROUND_XY(3, 10), str_credits_ryan, 20);
    core.draw_text(VRAM_BACKGROUND_XY(3, 11), str_credits_tommy, 20);
    break;
  case CREDIT_TEXT_ART:
    core.draw_text(VRAM_BACKGROUND_XY(3, 5), str_credits_dungeon_art, 20);
    core.draw_text(VRAM_BACKGROUND_XY(3, 6), str_credits_ryan, 20);

    core.draw_text(VRAM_BACKGROUND_XY(3, 8), str_credits_monster_art, 20);
    core.draw_text(VRAM_BACKGROUND_XY(3, 9), str_credits_ledu, 20);

    core.draw_text(VRAM_BACKGROUND_XY(3, 11), str_credits_title_art, 20);
    core.draw_text(VRAM_BACKGROUND_XY(3, 12), str_credits_mono, 20);
    break;
  case CREDIT_TEXT_PATREON:
    core.draw_text(VRAM_BACKGROUND_XY(3, 5), str_credits_patreon, 20);
    core.draw_text(VRAM_BACKGROUND_XY(1, 7), str_credits_nh_patreon, 20);
    core.draw_text(VRAM_BACKGROUND_XY(2, 9), str_credits_nh_patreon2, 20);
    core.draw_text(VRAM_BACKGROUND_XY(2, 10), str_credits_nh_patreon3, 20);
    break;
  case CREDIT_TEXT_THANK_YOU:
    core.draw_text(VRAM_BACKGROUND_XY(4, 8), str_credits_thank_you, 20);
    core.draw_text(VRAM_BACKGROUND_XY(4, 9), str_credits_for_playing, 20);
    break;
  }

  init_timer(credits_timer, 240);
  credits_text_page++;
}

void initialize_credits(void) {
  fade_out();
  game_state = GAME_STATE_CREDITS;
  credits_state = CREDITS_INIT;
  credits_text_page = CREDIT_TEXT_STORY1;
}

void init_credits(void) NONBANKED {
  SWITCH_ROM(BANK_1);
  initialize_credits();
}

void update_credits(void) BANKED {
  switch (credits_state) {
  case CREDITS_INIT:
    if (!fade_update())
      return;

    DISPLAY_OFF;
    toggle_sprites();

    scroll_bkg(0, 0);
    next_text_page();
    core.load_bg_palette(credits_palette, 0, 1);
    core.load_font();

    hide_window();

    fade_in();
    credits_state = CREDITS_FADE_IN;

    DISPLAY_ON;
    break;
  case CREDITS_FADE_IN:
    if (!fade_update())
      return;
    if (credits_text_page > CREDIT_TEXT_THANK_YOU)
      credits_state = CREDITS_FIN;
    else
      credits_state = CREDITS_HOLD;
    break;
  case CREDITS_HOLD:
    if (!update_timer(credits_timer))
      return;
    reset_timer(credits_timer);


    credits_state = CREDITS_FADE_OUT;
    fade_out();
    break;
  case CREDITS_FADE_OUT:
    if (!fade_update())
      return;

    DISPLAY_OFF;
    next_text_page();
    credits_state = CREDITS_FADE_IN;
    fade_in();
    DISPLAY_ON;

    break;
  case CREDITS_FIN:
    if (was_pressed(J_START)) {
      DISPLAY_OFF;
      init_title_screen();
      game_state = GAME_STATE_TITLE;
    }
    break;
  }
}
