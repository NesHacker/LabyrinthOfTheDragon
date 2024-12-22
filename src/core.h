#ifndef _CORE_H
#define _CORE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>

#include "data.h"
#include "palette.h"

/**
 * Enumeration of all available ROM banks for the game. Use these instead of
 * numbers to make it easier to understand constant data (map data, tilesets,
 * etc.).
 */
typedef enum GameRomBank {
  BANK_0,
  BANK_1,
  BANK_2,
  BANK_3,
  BANK_4,
  BANK_5,
  BANK_6,
  BANK_7,
  BANK_8,
  BANK_9,
  BANK_10,
  BANK_11,
  BANK_12,
  BANK_13,
  BANK_14,
  BANK_15,
} GameRomBank;

/**
 * Tile offset where the font tiles begin.
 */
#define FONT_OFFSET 0x80

/**
 * Tile id for the '0' character with backing when the font tileset is loaded.
 */
#define FONT_DIGIT_OFFSET 0xB0

/**
 * @return The tile id for the given digit.
 * @param d Digit to convert.
 */
#define FONT_DIGIT(d) ((d) + FONT_DIGIT_OFFSET)

/**
 * Tile id for the 'A' character in the font.
 */
#define FONT_A 0xC1

/**
 * Tile id for the 'B' character in the font.
 */
#define FONT_B 0xC2

/**
 * Tile id for the 'C' character in the font.
 */
#define FONT_C 0xC3

/**
 * Tile id for a space when the font tileset is loaded.
 */
#define FONT_SPACE 0xA0

/**
 * Tile id for a '/' character when the font tileset is loaded.
 */
#define FONT_SLASH 0xAF

/**
 * Tile id for a horizontal menu border (top normal, flip-y for bottom).
 */
#define FONT_BORDER_TOP 0x91

/**
 * Disables interrupts, turns of the LCD.
 */
#define lcd_off() do { \
    disable_interrupts(); \
    DISPLAY_OFF; \
  } while (0)

/**
 * Enables interrupts and turns the LCD on.
 */
#define lcd_on() do { \
    DISPLAY_ON; \
    enable_interrupts(); \
  } while (0)

/**
 * Creates a pointer to VRAM at the given offset.
 * @param offset Offset in VRAM for the pointer.
 */
#define VRAM(offset) (void *)(0x8000 + (offset))

/**
 * VRAM address to sprite tile data.
 */
#define VRAM_SPRITE_TILES (void *)0x8000

/**
 * VRAM address to shared bg/sprite tile data.
 */
#define VRAM_SHARED_TILES (void *)0x8800

/**
 * VRAM address to background tile data.
 */
#define VRAM_BG_TILES (void *)0x9000

/**
 * VRAM address for the background tilemap.
 */
#define VRAM_BACKGROUND (void *)0x9800

/**
 * VRAM address for the window tilemap.
 */
#define VRAM_WINDOW (void *)0x9C00

/**
 * Computes the background tilemap VRAM address for the given column and row.
 * @param x Column in VRAM.
 * @param y Row in VRAM.
 */
#define VRAM_BACKGROUND_XY(x, y)  (void *)(0x9800 + (x) + 0x20 * (y))

/**
 * Computes the window tilemap VRAM address for the given column and row.
 * @param x Column in VRAM.
 * @param y Row in VRAM.
 */
#define VRAM_WINDOW_XY(x, y)  (void *)(0x9C00 + (x) + 0x20 * (y))

/**
 * Adds a number of page (64 tiles) offsets to the given tile source pointer.
 * @param ptr Source data pointer.
 * @param n Number of pages by which offset.
 */
#define TILE_PAGE(ptr,n) (void *)((ptr) + 0x80 * 16 * ((n)-1))

/**
 * Computes an offset to add to a VRAM pointer to place it at the beginning of
 * tile of the next row on the next row of the background or window.
 * @param col Current column where VRAM is pointing.
 */
#define VRAM_ROW_OFFSET(col) (32 - (col))

/**
 * Random Seed to use for the game. If set to 0 then the game will generate a
 * seed value on the title while waiting for the player to begin the game.
 */
#define RANDOM_SEED 102

/**
 * Number of bytes per 8x8 pixel 2BBP tile.
 */
#define BYTES_PER_TILE 16

/**
 * Denotes that a tileset has a full 256 tiles.
 */
#define TILES_256 0

/**
 * Default VRAM location to write font tiles.
 */
#define VRAM_FONT_TILES VRAM_SHARED_TILES

/**
 * Default VRAM location to write battle tiles.
 */
#define VRAM_BATTLE_TILES (void *)0x9300

/**
 * Toggles sprites on/off.
 */
#define toggle_sprites() LCDC_REG ^= LCDCF_OBJON;

/**
 * Hides the window (but keeps it enabled).
 */
#define hide_window() move_win(0, 144)

/**
 * Universal direction type.
 */
typedef enum Direction {
  HERE, DOWN, UP, LEFT, RIGHT
} Direction;

/**
 * Basic timer structure for handling frame count timers.
 */
typedef struct Timer {
  uint8_t counter;
  uint8_t duration;
} Timer;

/**
 * Initializes a timer with the given duration.
 * @param t The timer to initialize.
 * @param d The duration for the timer.
 */
#define init_timer(t, d) do {\
    (t).duration = d; \
    (t).counter = d; \
  } while (0)

/**
 * Updates the timer for this frame.
 * @return `true` if the timer has completed.
 * @example
 * if (update_timer(my_timer)) {
 *   // Handle trigger code...
 *   reset_timer(my_timer)
 * }
 */
#define update_timer(t) ((--(t).counter) == 0)

/**
 * Resets a timer to let it count down again.
 */
#define reset_timer(t) (t).counter = (t).duration

/**
 * @param b Bit for the flag mask.
 * @return Mask for a flag at the given bit.
 */
#define FLAG(b) (1 << (b))

/**
 * Enumeration of names for each page in the global game flags.
 */
typedef enum FlagPage {
  /**
   * 8-bit bitfield of flags to use to determine if a chest is open.
   */
  FLAGS_CHEST_OPEN = 27,
  /**
   * Additional page of "chest open" flags.
   */
  FLAGS_CHEST_OPEN_B = 28,
  /**
   * 8-bit bitfield of flags to use to determine if a chest is locked.
   */
  FLAGS_CHEST_LOCKED = 29,
  /**
   * Additional page of "chest locked" flags.
   */
  FLAGS_CHEST_LOCKED_B = 30,
  /**
   * Uese these flags for testing.
   */
  TEST_FLAGS = 31
} FlagPage;

/**
 * Global game flags. Used as boolean indicators for various aspects of maps,
 * items, scenarios, etc.
 */
extern uint8_t flags[32];

/**
 * Determines if the flag on the given page is set.
 * @param page The flag page to check.
 * @param mask Bitmask to test.
 * @return `true` if all of the masked flag bits are set.
 */
inline bool check_flags(FlagPage page, uint8_t mask) {
  return flags[page] & mask;
}

/**
 * Sets global flags.
 * @param page The page for flag.
 * @param mask Mask for all flags to set.
 */
inline void set_flags(FlagPage page, uint8_t mask) {
  flags[page] |= mask;
}

/**
 * Unsets global flags.
 * @param page The page for flag.
 * @param mask Mask for all flags to unset.
 */
inline void unset_flags(FlagPage page, uint8_t mask) {
  flags[page] &= ~mask;
}

/**
 * Toggles global flags.
 * @param page The page for flag.
 * @param mask Mask for all flags to toggle.
 */
inline void toggle_flags(FlagPage page, uint8_t mask) {
  flags[page] ^= mask;
}

/**
 * Bitfield for buttons that are currently being held down.
 *
 * @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
 */
extern uint8_t joypad_down;

/**
 * Bitfield for buttons that were pressed as of this frame.
 *
 * @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
 */
extern uint8_t joypad_pressed;

/**
 * Bitfield for buttons that were released as of this frame.
 *
 * @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
 */
extern uint8_t joypad_released;

/**
 * @param b Button bitmask.
 * @return `true` If the button is down.
 */
#define is_down(b)      (joypad_down & (b))

/**
 * @param b Button bitmask.
 * @return `true` If the button was pressed this frame.
 */
#define was_pressed(b)  (joypad_pressed & (b))

/**
 * @param b Button bitmask.
 * @return `true` If the button was released this frame.
 */
#define was_released(b) (joypad_released & (b))

/**
 * General use blank palette (all RGB_WHITE).
 */
extern const palette_color_t blank_palette[4];

/**
 * Enumeration of all main states for the game.
 */
typedef enum GameState {
  GAME_STATE_TITLE,
  GAME_STATE_WORLD_MAP,
  GAME_STATE_BATTLE,
  GAME_STATE_MAIN_MENU,
  GAME_STATE_TEST = 0xFF,
} GameState;

/**
 * Main state for the game. Determines subsystem controller run during the
 * core game and rendering loops.
 */
extern GameState game_state;

/**
 * Core tileset structure.
 */
typedef struct Tileset {
  /**
   * Size (in tiles) for the tileset.
   */
  const uint8_t size;
  /**
   * Bank on which the tileset resides.
   */
  const GameRomBank bank;
  /**
   * Pointer to the data for the tileset.
   */
  const uint8_t *const data;
} Tileset;

/**
 * Enumeration of all supported tilemap types.
 */
typedef enum TilemapType {
  /**
   * Tilemap contains interleaved tile and attribute pairs (default).
   */
  TILEMAP_TILE_ATTR_PAIRS,
  /**
   * Tilemap contains tile data only.
   */
  TILEMAP_TILES_ONLY,
  /**
   * Tilemap contains all tiles, then all attributes.
   */
  TILEMAP_TILES_THEN_ATTRS,
  /**
   * Tilemap contains attributes only.
   */
  TILEMAP_ATTR_ONLY,
} TilemapType;

/**
 * Core tilemap structure.
 */
typedef struct Tilemap {
  /**
   * Width of the tilemap.
   */
  const uint8_t width;
  /**
   * Height of the tilemap.
   */
  const uint8_t height;
  /**
   * Bank where the data resides.
   */
  const GameRomBank bank;
  /**
   * Data for the tilemap.
   */
  const uint8_t *const data;
  /**
   * Structure for the tilemap data.
   */
  const TilemapType type;
} Tilemap;

/**
 * Core module wrapper.
 */
typedef struct Core {
  /**
   * Loads a tileset to the given destination.
   * @param s Tileset to load.
   * @param dst Destination pointer in VRAM.
   */
  const void (*load_tileset)(Tileset *s, uint8_t *dst);
  /**
   * Loads a number of tiles from a tileset.
   * @param s Tileset to load.
   * @param dst Destination to write tile data.
   * @param o Tile data offset (in tiles).
   * @param n Number of tiles to write.
   */
  const void (*load_tiles)(Tileset *s, uint8_t *dst, uint8_t o, uint8_t n);
  /**
   * Loads the core battle tileset.
   */
  const void (*load_battle_tiles)(void);
  /**
   * Loads the core font tileset.
   */
  const void (*load_font)(void);
  /**
   * Loads the font tiles for the given player class.
   */
  const void (*load_hero_tiles)(uint8_t player_class);
  /**
   * Loads all hero tiles.
   */
  const void (*load_all_heros)(void);
  /**
   * Loads the map objects tileset.
   */
  const void (*load_object_tiles)(void);
  /**
   * Draws a tilemap at the given location.
   * @param m Tilemap to load.
   * @param vram Destination pointer in VRAM.
   */
  const void (*draw_tilemap)(Tilemap s, uint8_t *dst);
  /**
   * Loads the given palette into the background palette set.
   * @param data Palette data to load.
   * @param index Starting palette index.
   * @param n Number of palettes to load.
   */
  const void (*load_bg_palette)(palette_color_t *data, uint8_t index, uint8_t n);
  /**
   * Loads the given palette into the foreground palette set.
   * @param data Palette data to load.
   * @param index Starting palette index.
   * @param n Number of palettes to load.
   */
  const void (*load_sprite_palette)(palette_color_t *data, uint8_t index, uint8_t n);

  /**
   * Clears the 32x32 tile background and fills tiles and attribute bytes with 0s.
   */
  const void (*clear_bg)(void);
  /**
   * Fill the background with the given tile and attribute.
   * @param tile Tile to fill.
   * @param attr Attribute to fill.
   */
  const void (*fill_bg)(uint8_t tile_id, uint8_t attr);
  /**
   * Draws text to the given VRAM location.
   * @param vram Pointer to VRAM where the text tiles should be written.
   * @param text The text to write.
   * @param max Maximum length for the text. Clears unused characters with spaces.
   */
  const void (*draw_text)(uint8_t *vram, const char *text, uint8_t max);
  /**
   * Prints a fraction to the screen at the given vram location.
   * @param vram Vram location to print the fraction.
   * @param n Numerator for the fraction.
   * @param d Denominator for the fraction.
   */
  const void (*print_fraction)(uint8_t *vram, uint16_t n, uint16_t d);
  /**
   * Hides all sprites by moving them to the origin.
   */
  const void (*hide_sprites)(void);
  /**
   * Fills a rect with the given tiles and attributes.
   * @param vram Vram location to begin the fill.
   * @param w Width for the fill.
   * @param h Height for the fill.
   * @param tile Tile to fill.
   * @param attr Attribute to fill.
   */
  const void (*fill)(
    uint8_t *vram, uint8_t w, uint8_t h, uint8_t tile, uint8_t attr);
  /**
   * Loads the dungeon tileset.
   */
  const void (*load_dungeon_tiles)(void);
} Core;

/**
 * The core module.
 */
extern const Core core;

#endif