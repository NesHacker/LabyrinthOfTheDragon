#ifndef _FLOOR_H
#define _FLOOR_H

#include "core.h"
#include "encounter.h"
#include "map.h"
#include "strings.h"

// IMPORTANT if you add a new floor, add an extern here
extern const Floor floor_test;
extern const Floor floor_test2;
extern const Floor floor_one;
extern const Floor floor_two;
extern const Floor floor_three;
extern const Floor floor_four;
extern const Floor floor_five;
extern const Floor floor_six;
extern const Floor floor_seven;
extern const Floor floor_eight;

/**
 * Set this as a custom chest handler to have the chest give the player a
 * magic key (see CHEST_5 example in `floor_test.c`).
 *
 * This will check to see if the chest is locked and if it isn't will cause
 * the chest to be opened and the player to gain a magic key.
 *
 * @param chest The chest being opened.
 * @return `true` If the chest was opened.
 */
bool chest_add_magic_key(Chest *chest);

/**
 * Set this as a custom chest handler to have the chest give the player the
 * torch (see CHEST_7 example in `floor_test.c`).
 *
 * This will check to see if the chest is locked and if it isn't will cause
 * the chest to be opened and the player to gain the torch.
 *
 * @param chest The chest being opened.
 * @return `true` If the chest was opened.
 */
bool chest_add_torch(Chest *chest);

// Chest contents: two potions and an ether
extern const Item chest_item_2pot_1eth[];

// Chest contents: one haste potion
extern const Item chest_item_haste_pot[];

// Chest contents: one regen potion
extern const Item chest_item_regen_pot[];

// Chest contents: potion
extern const Item chest_item_1pot[];

#endif
