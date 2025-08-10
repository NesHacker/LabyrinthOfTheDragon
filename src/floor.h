#ifndef _FLOOR_H
#define _FLOOR_H

#include "core.h"
#include "encounter.h"
#include "map.h"
#include "strings.h"

// Floor data (define these in your floor*.c files on whatever bank you want)

extern const Floor floor_test;
extern const Floor floor_test2;
extern const Floor floor1;
extern const Floor floor2;
extern const Floor floor3;
extern const Floor floor4;
extern const Floor floor5;
extern const Floor floor6;
extern const Floor floor7;
extern const Floor floor8;

// Floor bank entry externs (define these in `floor.banks.c`)

extern const FloorBank bank_floor1;
extern const FloorBank bank_floor2;
extern const FloorBank bank_floor3;
extern const FloorBank bank_floor4;
extern const FloorBank bank_floor5;
extern const FloorBank bank_floor6;
extern const FloorBank bank_floor7;
extern const FloorBank bank_floor8;

extern const FloorBank bank_floor_test;
extern const FloorBank bank_floor_test2;


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

/**
 * Handles some specialized logic for a couple chests in floor 7. I'm putting it
 * here as a temporary work around so I can avoid using bytes in ROM0.
 */
bool floor7_chest_on_open(const Chest *chest);


/**
 * Initializes the teleporter color animation.
 */
void init_teleporter_animation(uint8_t p, palette_color_t *colors) NONBANKED;

/**
 * Called to cycle the "bright" color of the given palette for the teleporter
 * color animation.
 */
void animate_teleporter_colors(uint8_t palette_number) BANKED;

extern const Item chest_item_2pot_1eth[];
extern const Item chest_item_haste_pot[];
extern const Item chest_item_regen_pot[];
extern const Item chest_item_1pot[];
extern const Item chest_item_2pots[];
extern const Item chest_item_1eth[];
extern const Item chest_item_1remedy[];
extern const Item chest_item_3potions[];
extern const Item chest_item_3ethers[];
extern const Item chest_item_1elixir[];
extern const Item chest_item_1atkup_1defup[];
extern const Item chest_item_3regen[];
extern const Item chest_item_3elixirs[];
extern const Item chest_item_3haste[];

#endif
