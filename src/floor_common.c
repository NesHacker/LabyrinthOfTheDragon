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
  } else {
    player.has_torch = true;
    map_textbox(str_maps_get_torch);
  }
  return true;
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

const Item chest_item_1elixer[] = {
  { ITEM_ELIXER, 1 },
  { END },
};

const Item chest_item_1atkup_1defup[] = {
  { ITEM_ATK_UP, 1 },
  { ITEM_DEF_UP, 1 },
  { END },
};
