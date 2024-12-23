#pragma bank 2

#include "floor.h"

// Add custom chest handlers here

bool chest_add_key(Chest *chest) {
  if (is_chest_locked(chest->id)) {
    map_textbox(str_maps_chest_locked);
    return false;
  } else {
    player.magic_keys++;
    map_textbox(str_maps_get_magic_key);
  }
  return true;
}

// As you need them add chest item layouts here

const Item chest_item_2pot_1eth[] = {
  { ITEM_POTION, 2 },
  { ITEM_ETHER, 1 },
  { END },
};

const Item chest_item_haste_pot[] = {
  { ITEM_HASTE, 1 },
  { END }
};

extern const Item chest_item_regen_pot[] = {
  { ITEM_REGEN, 1 },
  { END },
};
