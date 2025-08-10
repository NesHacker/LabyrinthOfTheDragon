#ifndef _ITEM_H
#define _ITEM_H

#include "core.h"

/**
 * Determines the healing strength of potions.
 */
#define POTION_HEAL_FACTOR 6

/**
 * Determines the healing strength of ethers.
 */
#define ETHER_HEAL_FACTOR 6

/**
 * Max number of items in the player's inventory.
 */
#define INVENTORY_LEN 8

/**
 * Item type identifier.
 */
typedef enum ItemId {
  ITEM_POTION,
  ITEM_ETHER,
  ITEM_REMEDY,
  ITEM_ATK_UP,
  ITEM_DEF_UP,
  ITEM_ELIXIR,
  ITEM_REGEN,
  ITEM_HASTE,
  ITEM_INVALID = 0xFF,
} ItemId;

/**
 * Inventory item structure.
 */
typedef struct Item {
  /**
   * Id / type of the item.
   */
  ItemId id;
  /**
   * How many of this item the player currently has.
   */
  uint8_t quantity;
  /**
   * Name for the item.
   */
  const char *name;
} Item;

/**
 * The player's inventory.
 */
extern Item inventory[8];

/**
 * Adds items to the player's inventory.
 * @param id Id of the item to add.
 * @param count Number of items to add (up to 99).
 */
inline void add_items(ItemId id, uint8_t count) {
  Item *item = inventory + id;
  item->quantity = item->quantity + count > 99 ? 99 : item->quantity + count;
}

/**
 * Removes a single item with the given id from the player's inventory.
 * @param id Id of the item to remove.
 * @return The number of items remaining;
 */
inline uint8_t remove_item(ItemId id) {
  Item *item = inventory + id;
  if (item->quantity == 0)
    return 0;
  return --item->quantity;
}

inline uint8_t clear_inventory(void) {
  Item *item = inventory;
  for (uint8_t k = 0; k < 8; k++, item++)
    inventory->quantity = 0;
}

/**
 * Whether or not an item can be used.
 */
bool can_use_item(ItemId id);

/**
 * Uses an item. Only performs the action for the item, inventory quantitiy
 * state must be managed with `remove_item`.
 * @param item Item to use.
 */
void use_item(ItemId id);

#endif