#ifndef _TILEMAPS_BANK00_H
#define _TILEMAPS_BANK00_H

#include <gb/gb.h>
#include <gbdk/incbin.h>

// Textbox: 20 x 6 x 2 (tiles & attributes)
INCBIN_EXTERN(tilemap_textbox)

// Main menu tilemaps (20x18 tile full screens, tile+attr pairs)
INCBIN_EXTERN(tilemap_title_screen)
INCBIN_EXTERN(tilemap_name_entry)
INCBIN_EXTERN(tilemap_save_select)

#endif
