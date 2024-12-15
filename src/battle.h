#ifndef _BATTLE_H
#define _BATTLE_H

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "data.h"
#include "encounter.h"
#include "flags.h"
#include "monster.h"
#include "player.h"
#include "tables.h"

// -----------------------------------------------------------------------------
// Constants & Macros
// -----------------------------------------------------------------------------

/**
 * First sprite id for the cursor icon.
 */
#define CURSOR_SPRITE 0

/**
 * Tile for the first cursor sprite.
 */
#define CURSOR_TILE 0x8A

/**
 * Sprite attribute for the cursor.
 */
#define CURSOR_ATTR 0x0F

/**
 * Clear tile to use when the battle controller is running.
 */
#define BATTLE_CLEAR_TILE 0xFF

/**
 * Clear attribute to use when the battle controller is running.
 */
#define BATTLE_CLEAR_ATTR 0x08

/**
 * Attribute to use for buff icon tiles.
 */
#define BUFF_ATTRIBUTE 0x0E

/**
 * Attribute to use for debuff icon tiles.
 */
#define DEBUFF_ATTRIBUTE 0x0F

/**
 * Tile id for the fight icon in the font tileset.
 */
#define FIGHT_ICON 0x30

/**
 * Tile id for the magic icon in the font tileset.
 */
#define MAGIC_ICON 0x34

/**
 * Tile id for the tech icon in the font tileset.
 */
#define TECH_ICON 0x40

/**
 * Tile id for the item icon in the font tileset.
 */
#define ITEM_ICON 0x3C

/**
 * Tile id for the summon icon in the font tileset.
 */
#define SUMMON_ICON 0x44

/**
 * Tile id for the flee icon in the font tileset.
 */
#define FLEE_ICON 0x38

/**
 * Tile id for the left tile of the "MP" label.
 */
#define MP_ICON_LEFT 0x98

/**
 * Tile id for the right tile of the "MP" label.
 */
#define MP_ICON_RIGHT 0x99

/**
 * Tile id for the left tile of the "SP" label.
 */
#define SP_ICON_LEFT 0x9A

/**
 * Tile id for the right tile of the "SP" label.
 */
#define SP_ICON_RIGHT 0x9B

/**
 * ROM bank in which most data and routines related to battle are stored. This
 * bank must be loaded in order for the battle system to properly function.
 * @see load_battle_bank()
 */
#define BATTLE_ROM_BANK 3

/**
 * Denotes the starting row in the background where the menu graphics begin.
 */
#define MENU_Y 11

/**
 * Denotes the starting row in the background where the submenu graphics begin.
 */
#define SUBMENU_Y 18

/**
 * Row where submenu body text begins.
 */
#define SUBMENU_TEXT_Y 19

/**
 * Column where submenu body text begins.
 */
#define SUBMENU_TEXT_X 2

/**
 * Tile for the HP bar graphics's left cap.
 */
#define HP_BAR_LEFT_CAP 0x50

/**
 * Tile with full pips for the HP bar.
 */
#define HP_BAR_FULL_PIPS 0x51

/**
 * Tile with empty pips for the HP bar.
 */
#define HP_BAR_EMPTY_PIPS 0x52

/**
 * Tile for the HP bar graphics's right cap.
 */
#define HP_BAR_RIGHT_CAP 0x5A


/**
 * VRAM address for the main menu summon text in the background.
 */
#define VRAM_SUMMON_NAME VRAM_BACKGROUND_XY(10, MENU_Y + 1)

/**
 * VRAM address for the starting tile of submenu text in the background.
 */
#define VRAM_SUBMENU_TEXT VRAM_BACKGROUND_XY(SUBMENU_TEXT_X, SUBMENU_TEXT_Y)

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/**
 * Main state enumeration for the battle system.
 */
typedef enum BattleState {
  /**
   * The player is on the main battle menu and choosing an action.
   */
  BATTLE_STATE_MENU,
  /**
   * Initiative is being rolled for the round.
   */
  BATTLE_ROLL_INITIATIVE,
  /**
   * Battle state is reset and the turn begins for the next entity in initiative
   * order.
   */
  BATTLE_NEXT_TURN,
  /**
   * Enitity status effects are updated, effects applied, and messages provided
   * for specific actions (e.g. effect falls off, regen health, etc.)
   */
  BATTLE_UPDATE_STATUS_EFFECTS,
  /**
   * A status effect message is being displayed.
   */
  BATTLE_STATUS_EFFECT_MESSAGE,
  /**
   * The player/monster's action is executed to set battle results state.
   */
  BATTLE_TAKE_ACTION,
  /**
   * The results of the action are being animated. This includes the preamble
   * text, anticipation pause, effect result text, effect animation, and final
   * message speed pause.
   */
  BATTLE_ANIMATE,
  /**
   * Performs cleanup after a round of battle (death logic, etc.)
   */
  BATTLE_ACTION_CLEANUP,
  /**
   * The "player flee" animation and message is being displayed.
   */
  BATTLE_PLAYER_FLED,
  /**
   * The player death animation is being displayed.
   */
  BATTLE_PLAYER_DIED,
  /**
   * End of round logic is being performed (skipping player turn etc.).
   */
  BATTLE_END_ROUND,
  /**
   * Battle is over and successful, calulates & displays rewards, etc.
   */
  BATTLE_SUCCESS,
  /**
   * Handles any level up messages or effects when the player levels up.
   */
  BATTLE_LEVEL_UP,
  /**
   * Exits battle mode and send the player back to the world map.
   */
  BATTLE_COMPLETE,
  /**
   * Battle is over/inactive and the update routine should do nothing if called.
   */
  BATTLE_INACTIVE,
} BattleState;

/**
 * Enumeration battle menus that can be displayed at the bottom of the screen.
 */
typedef enum BattleMenu {
  BATTLE_MENU_MAIN,
  BATTLE_MENU_FIGHT,
  BATTLE_MENU_ABILITY,
  BATTLE_MENU_ITEM,
  BATTLE_MENU_SUMMON,
} BattleMenu;

/**
 * Cursor position enumeration for various battle menus.
 */
typedef enum BattleCursor {
  BATTLE_CURSOR_MAIN_FIGHT,
  BATTLE_CURSOR_MAIN_ABILITY,
  BATTLE_CURSOR_MAIN_ITEM,
  BATTLE_CURSOR_MAIN_MAIN_SUMMON,
  BATTLE_CURSOR_MAIN_FLEE,
  BATTLE_CURSOR_ITEM_1,
  BATTLE_CURSOR_ITEM_2,
  BATTLE_CURSOR_ITEM_3,
  BATTLE_CURSOR_ITEM_4,
  BATTLE_CURSOR_ITEM_5,
  BATTLE_CURSOR_NO_ITEMS,
  BATTLE_CURSOR_MONSTER_1,
  BATTLE_CURSOR_MONSTER_2,
  BATTLE_CURSOR_MONSTER_3,
} BattleCursor;

/**
 * Enumeration of the three major layouts for the battle menu at the bottom of
 * the screen.
 */
typedef enum BattleMenuLayout {
  /**
   * Main menu w/ player stats.
   */
  BATTLE_MENU_LAYOUT_MAIN,
  /**
   * Submenus for "Fight", "Magic", "Tech", and "Summon".
   */
  BATTLE_MENU_LAYOUT_SUBMENU,
  /**
   * Text menu for describing the actions and results of rolls.
   */
  BATTLE_MENU_LAYOUT_TEXT,
} BattleMenuLayout;

/**
 * Enumeration of monster positions on the battle screen.
 */
typedef enum MonsterPosition {
  MONSTER_POSITION1,
  MONSTER_POSITION2,
  MONSTER_POSITION3,
} MonsterPosition;

/**
 * List of all battle animations that can be set as a result of actions.
 */
typedef enum BattleAnimation {
  BATTLE_ANIMATION_PAUSE,
} BattleAnimation;

// -----------------------------------------------------------------------------
// Externs & Prototypes
// -----------------------------------------------------------------------------

/**
 * Default palettes for the battle system.
 */
extern const palette_color_t battle_bg_palettes[];

/**
 * Default sprite palettes for the battle system.
 */
extern const palette_color_t battle_sprite_palettes[];

/**
 * Color palettes to use when animating monster death.
 */
extern const palette_color_t monster_death_colors[];

/**
 * Main state variable for the battle system.
 */
extern BattleState battle_state;

/**
 * Which menu is currently open on the battle system.
 */
extern BattleMenu battle_menu;

/**
 * Cursor state for battle system menus.
 */
extern BattleCursor battle_cursor;

/**
 * Number of selectable submenu items. General limit value for the abilities,
 * items, and summons submenus.
 */
extern uint8_t battle_num_submenu_items;

/**
 * Message to show prior to an action animation.
 * Ex: The kobold attacks with a stone axe...
 */
extern char battle_pre_message[64];

/**
 * Message to show after the action animation.
 * Ex: But they miss!
 */
extern char battle_post_message[64];

/**
 * Animation to play prior to the results of a player or monster action.
 */
extern BattleAnimation battle_action_effect;

/**
 * Initializes the battle system.
 */
void init_battle(void);

/**
 * Executed when battle is complete and before transitioning back to map mode.
 */
void cleanup_battle(void);

/**
 * Game loop update for the battle system.
 */
void update_battle(void);

/**
 * VBLANK draw updates for the battle system.
 */
void draw_battle(void);

// External data

extern const palette_color_t data_battle_bg_colors[];
extern const palette_color_t data_battle_sprite_colors[];

extern const Tilemap monster_layout_1;
extern const Tilemap monster_layout_2;
extern const Tilemap monster_layout_3s;
extern const Tilemap monster_layout_1m2s;
extern const Palette battle_bg_palette;
extern const Palette battle_sprite_palette;
extern const Tilemap battle_menu_tilemap;
extern const Tilemap battle_submenu_tilemap;
extern const Tilemap battle_textbox_tilemap;


#endif
