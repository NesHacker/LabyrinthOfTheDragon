#pragma bank 2

#include "floor.h"

bool chest_add_magic_key(Chest *chest) {
  if (is_chest_locked(chest->id)) {
    map_textbox(str_maps_chest_locked);
    return false;
  } else {
    player.got_magic_key = true;
    player.magic_keys++;
    map_textbox(str_maps_get_magic_key);
  }
  return true;
}

bool chest_add_torch(Chest *chest) {
  if (is_chest_locked(chest->id)) {
    map_textbox(str_maps_chest_locked);
    return false;
  } else if (player.has_torch) {
    map_textbox(str_maps_already_has_torch);
  } else {
    player.has_torch = true;
    map_textbox(str_maps_get_torch);
  }
  return true;
}

bool floor7_chest_on_open(const Chest *chest) {
  // The floor uses some "fake" chest graphics to provide a hint that for the
  // existence of a secret maze. Basically I need to ensure that those graphics
  // are toggled from closed -> opened if the corresponding "actual" chests are
  // opened by the player.
  if (chest->id == CHEST_5)
    set_tile_at(MAP_A, 21, 28, 0x1E);
  else if (chest->id == CHEST_6)
    set_tile_at(MAP_A, 14, 17, 0x0E);
  return false;
}

uint8_t portal_color_idx = 0;
Timer portal_color_timer;
#define MAX_PORTAL_COLOR_FRAMES 6

const palette_color_t portal_color[MAX_PORTAL_COLOR_FRAMES] = {
  RGB8(220, 0, 220),
  RGB8(180, 0, 240),
  RGB8(100, 120, 120),
  RGB8(20, 240, 0),
  RGB8(0, 200, 0),
  RGB8(100, 0, 100),
};

palette_color_t portal_color_palette[4] = {
  RGB8(220, 0, 220),
  RGB8(30, 45, 30),
  RGB8(40, 60, 40),
  RGB8(0, 0, 24),
};

void init_teleporter_animation(uint8_t p, const palette_color_t *colors) NONBANKED {
  init_timer(portal_color_timer, 4);
  for (uint8_t k = 0; k < 4; k++)
    portal_color_palette[k] = colors[4 * p + k];
}

void animate_teleporter_colors(uint8_t palette_number) BANKED {
  if (!update_timer(portal_color_timer))
    return;
  reset_timer(portal_color_timer);

  portal_color_idx++;
  if (portal_color_idx >= MAX_PORTAL_COLOR_FRAMES)
    portal_color_idx = 0;

  portal_color_palette[0] = portal_color[portal_color_idx];
  core.load_bg_palette(portal_color_palette, palette_number, 1);
}

const Item chest_item_2pot_1eth[] = {
  { ITEM_POTION, 2 },
  { ITEM_ETHER, 1 },
  { END }
};

const Item chest_item_haste_pot[] = {
  { ITEM_HASTE, 1 },
  { END }
};

const Item chest_item_regen_pot[] = {
  { ITEM_REGEN, 1 },
  { END }
};

const Item chest_item_1pot[] = {
  { ITEM_POTION, 1 },
  { END }
};

const Item chest_item_2pots[] = {
  { ITEM_POTION, 2 },
  { END }
};

const Item chest_item_1eth[] = {
  { ITEM_ETHER, 1 },
  { END }
};

const Item chest_item_1remedy[] = {
  { ITEM_REMEDY, 1 },
  { END }
};

const Item chest_item_3potions[] = {
  { ITEM_POTION, 3 },
  { END }
};

const Item chest_item_3ethers[] = {
  { ITEM_ETHER, 3 },
  { END },
};

const Item chest_item_1elixir[] = {
  { ITEM_ELIXIR, 1 },
  { END },
};

const Item chest_item_1atkup_1defup[] = {
  { ITEM_ATK_UP, 1 },
  { ITEM_DEF_UP, 1 },
  { END },
};

const Item chest_item_3regen[] = {
  { ITEM_REGEN, 3 },
  { END },
};

extern const Item chest_item_3elixirs[] = {
  { ITEM_ELIXIR, 3 },
  { END },
};

extern const Item chest_item_3haste[] = {
  { ITEM_HASTE, 3 },
  { END },
};