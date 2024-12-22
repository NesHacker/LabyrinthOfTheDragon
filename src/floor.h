#ifndef _FLOOR_H
#define _FLOOR_H

#include "core.h"
#include "encounter.h"
#include "map.h"

// IMPORTANT if you add a new floor, add an extern here
extern const Floor floor_test;

/**
 * Flags used to denote specific chests in a chest related bitfield.
 */
typedef enum ChestFlags {
  CHEST_FLAG_1 = FLAG(0),
  CHEST_FLAG_2 = FLAG(1),
  CHEST_FLAG_3 = FLAG(2),
  CHEST_FLAG_4 = FLAG(3),
  CHEST_FLAG_5 = FLAG(4),
  CHEST_FLAG_6 = FLAG(5),
  CHEST_FLAG_7 = FLAG(6),
  CHEST_FLAG_8 = FLAG(7),
} ChestFlags;

#endif
