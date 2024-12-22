#ifndef _FLOOR_H
#define _FLOOR_H

#include "core.h"
#include "map.h"

#define MAP_A 1
#define MAP_B 2
#define MAP_C 3
#define MAP_D 4

/**
 * Flags used to denote specific chests in a chest related bitfield.
 */
typedef enum ChestFlags {
  CHEST_1 = FLAG(0),
  CHEST_2 = FLAG(1),
  CHEST_3 = FLAG(2),
  CHEST_4 = FLAG(3),
  CHEST_5 = FLAG(4),
  CHEST_6 = FLAG(5),
  CHEST_7 = FLAG(6),
  CHEST_8 = FLAG(7),
} ChestFlags;

extern Tileset dungeon_tileset;

#endif
