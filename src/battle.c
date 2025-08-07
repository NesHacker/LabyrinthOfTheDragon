#pragma bank 3

#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "battle.h"
#include "core.h"
#include "credits.h"
#include "encounter.h"
#include "map.h"
#include "monster.h"
#include "palette.h"
#include "sound.h"
#include "strings.h"
#include "text_writer.h"

BattleState battle_state;
BattleMenu battle_menu;
uint8_t status_effect_x[3] = { 7, 7, 7 };

char battle_pre_message[128];
char battle_post_message[128];
bool skip_post_message = false;
void (*battle_sfx)(void);
char rewards_buf[128];
bool player_was_hit = false;

bool flee_sound_played = false;

BattleAnimation battle_animation;
AnimationState animation_state = ANIMATION_PREAMBLE;
Timer effect_delay_timer;
Timer monster_death_timer;
uint8_t monster_death_step = 0;
MonsterDeathAnimation monster_death_state = MONSTER_DEATH_START;
static Timer no_post_sfx_timer;
Timer death_delay_timer;
bool player_died = false;

/**
 * Used for timing continuous cursor movement.
 */
static Timer cursor_timer;

/**
 * Handles D-Pad input for battle menus. Allows the player to hold a direction
 * and have the cursor continue to move until they let go.
 * @param button Button to check.
 */
inline bool on_dpad(uint8_t button) {
  if (was_pressed(button)) {
    init_timer(cursor_timer, 20);
    return true;
  } else if (is_down(button)) {
    if (!update_timer(cursor_timer))
      return false;
    init_timer(cursor_timer, 9);
    return true;
  }
  return false;
}

/**
 * Timer used to animate screen shakes when the player is hit.
 */
static Timer screen_shake_timer;

/**
 * Used to animate a shaking screen when hit.
 */
static uint8_t screen_shake_index = 0;

/**
 * Screen scroll x positions for the screen shake effect.
 */
static const int8_t screen_shake[] = { 6, -6, 4, -4, 0 };

/**
 * Whether or not the screen shake effect is active.
 */
static bool is_screen_shaking = false;

/**
 * Finds the monster currently selected by the screen cursor.
 * @return Monster intance for the selected monster.
 */
static Monster *get_monster_at_cursor(void) {
  uint8_t monster_idx = 0;
  switch (battle_menu.screen_cursor) {
  case BATTLE_CURSOR_MONSTER_1:
    monster_idx = 0;
    break;
  case BATTLE_CURSOR_MONSTER_2:
    monster_idx = 1;
    break;
  case BATTLE_CURSOR_MONSTER_3:
    monster_idx = 2;
    break;
  }
  return get_monster(monster_idx);
}

/**
 * Confirms that the player has chosen the default "fight" action and begins the
 * next round of combat.
 */
static void confirm_fight(void) {
  play_sound(sfx_menu_move);
  set_player_fight(get_monster_at_cursor());
  battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms that the player has chosen an ability action and begins the next
 * round of combat.
 */
static void confirm_ability(const Ability *ability) {
  play_sound(sfx_menu_move);
  if (ability->target_type == TARGET_SINGLE)
    set_player_ability(ability, get_monster_at_cursor());
  else
    set_player_ability(ability, NULL);
  battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms that the player has chosen to use an item and begins the next round
 * of combat.
 */
static void confirm_item(ItemId item_id) {
  play_sound(sfx_menu_move);
  set_player_item(item_id);
  battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Confirms that the player has selected the "flee" action from the main menu
 * and begins the next round of combat.
 */
static void confirm_flee(void) {
  play_sound(sfx_menu_move);
  set_player_flee();
  battle_state = BATTLE_ROLL_INITIATIVE;
}

/**
 * Determines the x position for the HP bar based on the given monster position
 * and the current monster layout.
 * @param pos Position of the monster on the screen.
 * @return The x position for the HP bar.
 */
static uint8_t get_hp_bar_x(MonsterPosition pos) {
  switch (encounter.layout) {
  case MONSTER_LAYOUT_1:
    // 1 Monster  - (6, 9)
    return 7;
  case MONSTER_LAYOUT_2:
    // 2 Monsters - (2, 9), (11, 9)
    return (pos == MONSTER_POSITION1) ? 3 : 12;
  case MONSTER_LAYOUT_3S:
    // 3 Monsters - (0, 9), (6, 9), (12, 9)
    switch (pos) {
    case MONSTER_POSITION1: return 1;
    case MONSTER_POSITION2: return 7;
    case MONSTER_POSITION3: return 13;
    }
  case MONSTER_LAYOUT_1M_2S:
    // 1 md + 2 sm - (0) (7) (13)
    switch (pos) {
    case MONSTER_POSITION1: return 1;
    case MONSTER_POSITION2: return 8;
    case MONSTER_POSITION3: return 14;
    }
  }
  return 6;
}

/**
 * Loads the graphics and palettes for a monster instance.
 * @param p Position of the monsters on the battle screen.
 * @param m Monster instance to load.
 */
static void load_monster_graphics(MonsterPosition p, Monster *m) {
  if (!m->active)
    return;

  const Tileset *tileset = m->tileset;

  switch (p) {
  case MONSTER_POSITION1:
    VBK_REG = VBK_BANK_0;
    core.load_tileset(tileset, (void *)0x9000);
    update_bg_palettes(1, 1, m->palette);
    break;
  case MONSTER_POSITION2:
    VBK_REG = VBK_BANK_0;
    // 30 -> VRAM_BG_TILES + page_offset
    core.load_tiles(tileset, (void *)0x9620, 0, 30);
    // 68 -> VRAM_SHARED_TILES
    core.load_tiles(tileset, (void *)0x8800, 30, 68);
    update_bg_palettes(2, 1, m->palette);
    break;
  case MONSTER_POSITION3:
    // 60 -> VRAM_SHARED_TILES + 68 * 16
    VBK_REG = VBK_BANK_0;
    core.load_tiles(tileset, (void *)0x8C40, 0, 60);
    // 38 -> VRAM_BG_TILES BANK[2]
    VBK_REG = VBK_BANK_1;
    core.load_tiles(tileset, (void *)0x9000, 60, 38);
    update_bg_palettes(3, 1, m->palette);
    break;
  }
}

/**
 * Toggles the attributes of the HP bar so they match a monster's palettes so
 * it can be faded out along with the monster upon death.
 */
static void toggle_hp_bar_palette(MonsterPosition pos) {
  const uint8_t x = get_hp_bar_x(pos);
  const uint8_t y = 9;
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);
  VBK_REG = VBK_ATTRIBUTES;
  for (uint8_t k = 0; k < 5; k++, vram++) {
    set_vram_byte(vram, (pos + 1) | 0x08);
  }
}

/**
 * Draws an HP bar for the monster at a given position.
 * @param pos Position of the monster on the battle screen.
 * @param hp Curent HP for the monster.
 * @param max Max HP for the monster.
 */
static void draw_hp_bar(MonsterPosition pos, uint16_t hp, uint16_t max) {
  // Determine the x, y position for the bar
  const uint8_t x = get_hp_bar_x(pos);
  const uint8_t y = 9;
  uint8_t col = 0;
  uint8_t *vram = VRAM_BACKGROUND_XY(x, y);

  // Calculate the number of pips based on the ratio of HP to Max HP
  uint32_t k = 40;
  k *= hp;
  k /= max;
  uint8_t p = k;

  // Update the attributes based on HP percentage
  // If p <= 13, then HP <= (1/3) * max, so set critical palette
  VBK_REG = VBK_ATTRIBUTES;
  for (col = 0; col < 5; col++)
    set_vram_byte(vram++, p <= 13 ? 0b00001101 : 0b00001100);

  // Draw the graphics
  vram -= 5;
  VBK_REG = VBK_TILES;
  col = 0;

  // Fill full bar sprites until the pips value falls below 8
  while (p >= 8) {
    set_vram_byte(vram++, HP_BAR_FULL_PIPS);
    p -= 8;
    col++;
  }

  // If we've not filled all the pip sprites, then draw the next based on the
  // remaining pips
  if (col < 5) {
    set_vram_byte(vram++, HP_BAR_EMPTY_PIPS + p);
    col++;
  }

  // Draw any remaining pip sprites as "empty"
  while (col < 5) {
    set_vram_byte(vram++, HP_BAR_EMPTY_PIPS);
    col++;
  }
}

/**
 * Determines the starting x position for a monster's status effect tiles.
 * @param pos Monster position.
 * @return The starting x position for the status effects.
 */
static uint8_t get_status_effect_x(MonsterPosition pos) {
  const uint8_t offscreen = 0x17;

  switch (encounter.layout) {
  case MONSTER_LAYOUT_1:
    switch (pos) {
    case MONSTER_POSITION1: return 7;
    default: return offscreen;
    }
  case MONSTER_LAYOUT_2:
    switch (pos) {
    case MONSTER_POSITION1: return 3;
    case MONSTER_POSITION2: return 12;
    default: return offscreen;
    }
  case MONSTER_LAYOUT_3S:
    switch (pos) {
    case MONSTER_POSITION1: return 1;
    case MONSTER_POSITION2: return 7;
    case MONSTER_POSITION3: return 13;
    }
  case MONSTER_LAYOUT_1M_2S:
    switch (pos) {
    case MONSTER_POSITION1: return 1;
    case MONSTER_POSITION2: return 8;
    case MONSTER_POSITION3: return 14;
    }
  }
  return offscreen;
}

/**
 * Redraws all status effects for the player.
 */
static void redraw_player_status_effects(void) {
  uint8_t *vram = VRAM_BACKGROUND_XY(12, 15);
  StatusEffectInstance *effect = encounter.player_status_effects;
  uint8_t p = 0;

  VBK_REG = VBK_TILES;

  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
    if (!effect->active)
      continue;
    set_vram_byte(vram++, 0x60 + effect->effect);
    p++;
  }

  for (uint8_t k = p; k < MAX_ACTIVE_EFFECTS; k++)
    set_vram_byte(vram++, FONT_SPACE);
}

/**
 * Redraw status effects for all monsters.
 */
static void redraw_monster_status_effects(void) {
  Monster *monster = encounter.monsters;
  const uint8_t frame_offset = 0x60;

  for (uint8_t m = MONSTER_POSITION1; m <= MONSTER_POSITION3; m++, monster++) {
    uint8_t *vram = VRAM_BACKGROUND_XY(status_effect_x[m], 10);
    uint8_t *clear_vram = vram;

    for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++) {
      VBK_REG = VBK_TILES;
      set_vram_byte(clear_vram, BATTLE_CLEAR_TILE);
      VBK_REG = VBK_ATTRIBUTES;
      set_vram_byte(clear_vram++, BATTLE_CLEAR_ATTR);
    }

    if (!monster->active)
      continue;

    StatusEffectInstance *effect = monster->status_effects;
    uint8_t p = 0;

    for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
      if (!effect->active)
        continue;
      const uint8_t e = effect->effect;
      uint8_t tile = frame_offset + e;
      uint8_t attr = e < 8 ? DEBUFF_ATTRIBUTE : BUFF_ATTRIBUTE;
      VBK_REG = VBK_TILES;
      set_vram_byte(vram, tile);
      VBK_REG = VBK_ATTRIBUTES;
      set_vram_byte(vram++, attr);
      p++;
    }
  }
}

/**
 * Moves cursor sprites offscreen to hide them.
 */
static void hide_cursor(void) {
  move_sprite(CURSOR_SPRITE + 0, 0, 0);
  move_sprite(CURSOR_SPRITE + 1, 0, 0);
  move_sprite(CURSOR_SPRITE + 2, 0, 0);
  move_sprite(CURSOR_SPRITE + 3, 0, 0);
}

/**
 * Moves the cursor sprites to the given column and row on the screen.
 * @param col Column for the first sprite.
 * @param row Row for the first sprite.
 */
static void move_cursor_sprites(uint8_t col, uint8_t row) {
  uint8_t x = (col + 1) << 3;
  uint8_t y = ((row + 2) << 3) - 1;
  move_sprite(CURSOR_SPRITE + 0, x, y);
  move_sprite(CURSOR_SPRITE + 1, x + 8, y);
  move_sprite(CURSOR_SPRITE + 2, x, y + 8);
  move_sprite(CURSOR_SPRITE + 3, x + 8, y + 8);
}

/**
 * Moves the battle cursor to the given cursor position.
 * @param c Cursor position to set.
 */
static void move_screen_cursor(BattleScreenCursor c) {
  if (c != battle_menu.screen_cursor)
    play_sound(sfx_menu_move);

  battle_menu.screen_cursor = c;

  switch (battle_menu.screen_cursor) {
  case BATTLE_CURSOR_MAIN_FIGHT:
    move_cursor_sprites(0, 13);
    break;
  case BATTLE_CURSOR_MAIN_ABILITY:
    move_cursor_sprites(0, 14);
    break;
  case BATTLE_CURSOR_MAIN_ITEM:
    move_cursor_sprites(0, 15);
    break;
  case BATTLE_CURSOR_MAIN_FLEE:
    move_cursor_sprites(0, 16);
    break;
  case BATTLE_CURSOR_ITEM_1:
    move_cursor_sprites(0, 13);
    break;
  case BATTLE_CURSOR_ITEM_2:
    move_cursor_sprites(0, 14);
    break;
  case BATTLE_CURSOR_ITEM_3:
    move_cursor_sprites(0, 15);
    break;
  case BATTLE_CURSOR_ITEM_4:
    move_cursor_sprites(0, 16);
    break;
  case BATTLE_CURSOR_NO_ITEMS:
    hide_cursor();
    break;
  case BATTLE_CURSOR_MONSTER_1:
    switch (encounter.layout) {
    case MONSTER_LAYOUT_1:
      move_cursor_sprites(5, 5);
      break;
    case MONSTER_LAYOUT_2:
      move_cursor_sprites(1, 5);
      break;
    case MONSTER_LAYOUT_3S:
    case MONSTER_LAYOUT_1M_2S:
      move_cursor_sprites(0, 5);
      break;
    }
    break;
  case BATTLE_CURSOR_MONSTER_2:
    switch (encounter.layout) {
    case MONSTER_LAYOUT_1:
      move_cursor_sprites(5, 5);
      break;
    case MONSTER_LAYOUT_2:
      move_cursor_sprites(10, 5);
      break;
    case MONSTER_LAYOUT_3S:
      move_cursor_sprites(6, 5);
      break;
    case MONSTER_LAYOUT_1M_2S:
      move_cursor_sprites(7, 5);
      break;
    }
    break;
  case BATTLE_CURSOR_MONSTER_3:
    switch (encounter.layout) {
    case MONSTER_LAYOUT_1:
      move_cursor_sprites(5, 5);
      break;
    case MONSTER_LAYOUT_2:
      move_cursor_sprites(10, 5);
      break;
    case MONSTER_LAYOUT_3S:
      move_cursor_sprites(12, 5);
      break;
    case MONSTER_LAYOUT_1M_2S:
      move_cursor_sprites(13, 5);
      break;
    }
    break;
  }
}

/**
 * Moves the screen cursor without executing the "menu move" sound effect.
 */
static inline void move_screen_cursor_no_sound(BattleScreenCursor c) {
  battle_menu.screen_cursor = c;
  move_screen_cursor(c);
}

/**
 * Initializes graphics and state for the encounter and monsters.
 */
static void battle_init_encounter(void) {
  // Initialize graphics for the monsters in the encounter
  switch (encounter.layout) {
  case MONSTER_LAYOUT_1:
    core.draw_tilemap(tilemap_monster_layout_1, VRAM_BACKGROUND);
    break;
  case MONSTER_LAYOUT_2:
    core.draw_tilemap(tilemap_monster_layout_2, VRAM_BACKGROUND);
    break;
  case MONSTER_LAYOUT_3S:
    core.draw_tilemap(tilemap_monster_layout_3s, VRAM_BACKGROUND);
    break;
  case MONSTER_LAYOUT_1M_2S:
    core.draw_tilemap(tilemap_monster_layout_1m2s, VRAM_BACKGROUND);
    break;
  }

  Monster *monster = encounter.monsters;
  load_monster_graphics(MONSTER_POSITION1, monster++);
  load_monster_graphics(MONSTER_POSITION2, monster++);
  load_monster_graphics(MONSTER_POSITION3, monster);
}

/**
 * Updates the player's HP fraction.
 */
static void update_player_hp(void) {
  uint8_t *vram = VRAM_BACKGROUND_XY(
    BATTLE_HP_X,
    BATTLE_HP_Y
  );
  core.print_fraction(vram, player.hp, player.max_hp);
}

/**
 * Updates the player's MP fraction on the main menu.
 */
static void update_player_mp(void) {
  uint8_t *vram = VRAM_BACKGROUND_XY(
    BATTLE_MP_X,
    BATTLE_MP_Y
  );
  core.print_fraction(vram, player.sp, player.max_sp);
}

/**
 * Updates the menu and resource to reflect whether or not a player has a
 * magic or martial class.
 */
static void set_magic_or_martial(void) {
  uint8_t menu_icon, resource_icon;
  if (is_magic_class()) {
    menu_icon = MAGIC_ICON;
    resource_icon = MP_ICON_LEFT;
  } else {
    menu_icon = TECH_ICON;
    resource_icon = SP_ICON_LEFT;
  }
  uint8_t *vram = VRAM_BACKGROUND_XY(2, MENU_Y + 2);
  VBK_REG = VBK_TILES;
  for (uint8_t k = 0; k < 4; k++)
    set_vram_byte(vram++, menu_icon + k);
  vram = VRAM_BACKGROUND_XY(BATTLE_MP_X - 3, BATTLE_MP_Y);
  set_vram_byte(vram++, resource_icon++);
  set_vram_byte(vram, resource_icon);
}

/**
 * Attempts to move the cursor to one of the monsters at the given indices.
 * Doesn't move the cursor if neither are active.
 * @param a Index for the first enemy to select if active.
 * @param b Index for the second enemy to select if active.
 */
static void select_monster(uint8_t a, uint8_t b) {
  Monster *first = encounter.monsters + a;
  Monster *second = encounter.monsters + b;
  if (first->active) {
    move_screen_cursor(BATTLE_CURSOR_MONSTER_1 + a);
    return;
  }
  if (second->active) {
    move_screen_cursor(BATTLE_CURSOR_MONSTER_1 + b);
    return;
  }
}

/**
 * Moves the cursor to select the previous enemy in the list.
 */
static void select_prev_enemy(void) {
  if (encounter.layout == MONSTER_LAYOUT_1)
    return;
  switch (battle_menu.screen_cursor) {
  case BATTLE_CURSOR_MONSTER_1:
    select_monster(2, 1);
    return;
  case BATTLE_CURSOR_MONSTER_2:
    select_monster(0, 2);
    return;
  case BATTLE_CURSOR_MONSTER_3:
    select_monster(1, 0);
    break;
  }
}

/**
 * Moves the cursor to select the next enemy in the list.
 */
static void select_next_enemy(void) {
  if (encounter.layout == MONSTER_LAYOUT_1)
    return;
  switch (battle_menu.screen_cursor) {
  case BATTLE_CURSOR_MONSTER_1:
    select_monster(1, 2);
    return;
  case BATTLE_CURSOR_MONSTER_2:
    select_monster(2, 0);
    return;
  case BATTLE_CURSOR_MONSTER_3:
    select_monster(0, 1);
    break;
  }
}

/**
 * Draws the "EMPTY..." message in the middle of an empty battle_menu.
 */
static inline void draw_menu_empty_text(void) {
  core.draw_text(VRAM_BACKGROUND_XY(7, SUBMENU_Y + 2), str_misc_empty, 6);
}

/**
 * Draws the submenu heading based on which battle menu is currently set.
 */
static inline void draw_submenu_heading(void) {
  uint8_t start_tile;
  uint8_t len;
  switch (battle_menu.active_menu) {
  case BATTLE_MENU_ABILITY:
    start_tile = is_magic_class() ? MAGIC_ICON : TECH_ICON;
    len = 4;
    break;
  case BATTLE_MENU_ITEM:
    start_tile = ITEM_ICON;
    len = 4;
    break;
  default:
    start_tile = SUMMON_ICON;
    len = 5;
  }
  uint8_t *vram = VRAM_BACKGROUND_XY(1, SUBMENU_Y);
  uint8_t k;
  for (k = 0; k < len; k++, start_tile++, vram++)
    set_vram_byte(vram, start_tile);
  while (k++ < 5)
    set_vram_byte(vram++, FONT_BORDER_TOP);
}

/**
 * Pre renders entry text for the player's inventory into a buffer. This allows
 * the items to be quickly rendered when the submenu is opened.
 */
static void render_item_text(void) {
  const char *format = " %s        x%2u";

  Item *item = inventory;
  uint8_t p = 0;
  for (uint8_t k = 0; k < INVENTORY_LEN; k++, item++) {
    if (!item->quantity)
      continue;
    battle_menu.item_at[p] = item->id;
    sprintf(battle_menu.item_text[p], format, item->name, item->quantity);
    p++;
  }

  battle_menu.inventory_entries = p;

  while (p < INVENTORY_LEN) {
    for (uint8_t k = 0; k < 18; k++) {
      battle_menu.item_text[p][k] = (char)FONT_SPACE;
      battle_menu.item_at[p] = ITEM_INVALID;
    }
    p++;
  }
}

/**
 * Re-renders the item at the current submenu cursor. Primarily used to redraw
 * item quantities when they are used.
 */
static void update_item_text_at_cursor(void) {
  const char *format = " %s        x%2u";
  const uint8_t cursor = battle_menu.cursor;
  Item *item = inventory + battle_menu.item_at[cursor];
  sprintf(battle_menu.item_text[cursor], format, item->name, item->quantity);
}

/**
 * Removes the text for the item at the cursor if the last one was used.
 */
static void remove_item_text_at_cursor(void) {
  const uint8_t cursor = battle_menu.cursor;
  const uint8_t entries = battle_menu.inventory_entries - 1;

  for (uint8_t c = 0; c < 18; c++) {
    battle_menu.item_text[cursor][c] = (char)FONT_SPACE;
    battle_menu.item_at[cursor] = ITEM_INVALID;
  }

  for (uint8_t k = cursor; k < entries; k++) {
    battle_menu.item_at[k] = battle_menu.item_at[k + 1];
    for (uint8_t c = 0; c < 18; c++)
      battle_menu.item_text[k][c] = battle_menu.item_text[k + 1][c];
  }

  battle_menu.inventory_entries = entries;
}

/**
 * Pre renders the entry text for abilities to a buffer so they can be rendered
 * quickly.
 */
static void render_ability_text(void) {
  uint8_t a = 0;

  const char *format = is_magic_class() ?
    " %s\x18\x19%2u" :
    " %s\x1A\x1B%2u";

  for (uint8_t k = 0; k < player_num_abilities; k++, a++) {
    if (!player_abilities[k]->id) {
      break;
    }
    const char *name = player_abilities[k]->name;
    uint8_t sp = player_abilities[k]->sp_cost;
    sprintf(battle_menu.ability_text[a], format, name, sp);
  }

  while (a < MAX_ABILITIES) {
    for (uint8_t k = 0; k < 18; k++)
      battle_menu.ability_text[a][k] = (char)FONT_SPACE;
    a++;
  }
}

/**
 * Draws scroll arrows for the submenu based on its current state.
 */
static void draw_submenu_scroll_arrows(void) {
  const uint8_t arrow_tile = 0xFC;
  const uint8_t border_tile = 0x91;

  if (battle_menu.max_scroll == 0) {
    set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y), border_tile);
    set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y + 5), border_tile);
    return;
  }

  if (battle_menu.scroll == 0) {
    set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y), border_tile);
    set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y + 5), arrow_tile);
    return;
  }

  if (battle_menu.scroll < battle_menu.max_scroll) {
    set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y), arrow_tile);
    set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y + 5), arrow_tile);
    return;
  }

  set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y), arrow_tile);
  set_vram_byte(VRAM_BACKGROUND_XY(0x12, SUBMENU_Y + 5), border_tile);
}

/**
 * Redraws submenu text based on the current subemenu state and content buffers.
 */
static void redraw_submenu_text(void) {
  if (
    battle_menu.active_menu != BATTLE_MENU_ABILITY &&
    battle_menu.active_menu != BATTLE_MENU_ITEM
  ) {
    return;
  }

  const uint8_t max = battle_menu.entries < 4 ? battle_menu.entries : 4;
  uint8_t *vram = VRAM_SUBMENU_TEXT;

  if (battle_menu.active_menu == BATTLE_MENU_ABILITY) {
    for (uint8_t k = 0; k < max; k++, vram += 0x20) {
      const uint8_t line = k + battle_menu.scroll;
      core.draw_text(vram, battle_menu.ability_text[line], 18);
    }
  } else {
    for (uint8_t k = 0; k < max; k++, vram += 0x20) {
      const uint8_t line = k + battle_menu.scroll;
      core.draw_text(vram, battle_menu.item_text[line], 18);
    }
  }

  if (max >= 4)
    return;

  for (uint8_t j = 0; j < 4 - max; j++, vram += 32 - 18) {
    for (uint8_t x = 0; x < 18; x++, vram++)
      set_vram_byte(vram, FONT_SPACE);
  }
}

/**
 * Initalizes a submenu for the given entry lines and number of entries.
 * @param lines Line buffers to draw.
 * @param entries Total number of entries for the battle_menu.
 */
static void load_submenu(BattleMenuType menu) {
  if (
    battle_menu.active_menu != BATTLE_MENU_ABILITY &&
    battle_menu.active_menu != BATTLE_MENU_ITEM
  ) {
    return;
  }

  uint8_t entries = (menu == BATTLE_MENU_ABILITY) ?
    player_num_abilities :
    battle_menu.inventory_entries;

  battle_menu.active_menu = menu;
  battle_menu.entries = entries;
  battle_menu.cursor = 0;
  battle_menu.scroll = 0;
  battle_menu.max_scroll = entries <= 4 ? 0 : entries - 4;

  draw_submenu_heading();
  draw_submenu_scroll_arrows();

  if (battle_menu.entries == 0) {
    hide_cursor();
    uint8_t *vram = VRAM_BACKGROUND_XY(SUBMENU_TEXT_X, SUBMENU_TEXT_Y);
    for (uint8_t y = 0; y < 4; y++) {
      for (uint8_t x = 0; x < 18; x++)
        set_vram_byte(vram++, FONT_SPACE);
      vram += 14;
    }
    draw_menu_empty_text();
    move_screen_cursor(BATTLE_CURSOR_NO_ITEMS);
    return;
  } else {
    redraw_submenu_text();
    move_screen_cursor(BATTLE_CURSOR_ITEM_1);
  }
}

/**
 * Moves the submenu cursor up based on the current submenu state.
 */
static void submenu_cursor_up(void) {
  if (battle_menu.entries == 0) {
    move_screen_cursor(BATTLE_CURSOR_NO_ITEMS);
    return;
  }

  if (battle_menu.cursor == 0) {
    if (battle_menu.entries == 1)
      return;
    battle_menu.cursor = battle_menu.entries - 1;
    if (battle_menu.entries <= 4)
      move_screen_cursor(BATTLE_CURSOR_ITEM_1 + battle_menu.cursor);
    else {
      battle_menu.scroll = battle_menu.entries - 4;
      move_screen_cursor(BATTLE_CURSOR_ITEM_4);
    }
    redraw_submenu_text();
    draw_submenu_scroll_arrows();
    return;
  }

  battle_menu.cursor--;

  if (battle_menu.screen_cursor > BATTLE_CURSOR_ITEM_1) {
    move_screen_cursor(battle_menu.screen_cursor - 1);
  } else {
    if (battle_menu.scroll > 0) {
      battle_menu.scroll--;
      redraw_submenu_text();
      draw_submenu_scroll_arrows();
      play_sound(sfx_menu_move);
    }
  }
}

/**
 * Moves the submenu cursor down based on the current submenu state.
 */
static void submenu_cursor_down(void) {
  if (battle_menu.entries == 0) {
    move_screen_cursor(BATTLE_CURSOR_NO_ITEMS);
    return;
  }

  if (battle_menu.cursor == battle_menu.entries - 1) {
    if (battle_menu.entries == 1)
      return;

    battle_menu.cursor = 0;
    battle_menu.scroll = 0;
    move_screen_cursor(BATTLE_CURSOR_ITEM_1);
    redraw_submenu_text();
    draw_submenu_scroll_arrows();
    play_sound(sfx_menu_move);
    return;
  }

  battle_menu.cursor++;

  if (battle_menu.screen_cursor < BATTLE_CURSOR_ITEM_4) {
    move_screen_cursor(battle_menu.screen_cursor + 1);
  } else {
    if (battle_menu.scroll < battle_menu.max_scroll) {
      battle_menu.scroll++;
      redraw_submenu_text();
      draw_submenu_scroll_arrows();
      play_sound(sfx_menu_move);
    }
  }
}

/**
 * Opens the given battle menu and updates the battle menu state.
 * @param m Battle menu to open.
 */
static void open_battle_menu(BattleMenuType m) {
  uint8_t prev_menu = battle_menu.active_menu;
  battle_menu.active_menu = m;
  switch (m) {
  case BATTLE_MENU_MAIN:
    switch (prev_menu) {
    case BATTLE_MENU_ABILITY:
      move_screen_cursor(BATTLE_CURSOR_MAIN_ABILITY);
      break;
    case BATTLE_MENU_ITEM:
      move_screen_cursor(BATTLE_CURSOR_MAIN_ITEM);
      break;
    default:
      move_screen_cursor(BATTLE_CURSOR_MAIN_FIGHT);
    }
    break;
  case BATTLE_ABILITY_MONSTER_SELECT:
  case BATTLE_MENU_FIGHT:
    Monster *monster = encounter.monsters;
    for (uint8_t pos = 0; pos < 3; pos++, monster++) {
      if (monster->active) {
        move_screen_cursor(BATTLE_CURSOR_MONSTER_1 + pos);
        return;
      }
    }
    // We shouldn't reach here unless testing or an error happens
    battle_menu.active_menu = BATTLE_MENU_MAIN;
    move_screen_cursor(prev_menu - 1);
    break;
  case BATTLE_MENU_ABILITY:
    load_submenu(BATTLE_MENU_ABILITY);
    break;
  case BATTLE_MENU_ITEM:
    load_submenu(BATTLE_MENU_ITEM);
    break;
  }
}

/**
 * Moves the main menu cursor up one position.
 */
static void main_menu_cursor_up(void) {
  if (battle_menu.screen_cursor == BATTLE_CURSOR_MAIN_FIGHT)
    move_screen_cursor(BATTLE_CURSOR_MAIN_FLEE);
  else
    move_screen_cursor(battle_menu.screen_cursor - 1);
}

/**
 * Moves the main menuy cursor down one position.
 */
static void main_menu_cursor_down(void) {
  if (battle_menu.screen_cursor == BATTLE_CURSOR_MAIN_FLEE)
    move_screen_cursor(BATTLE_CURSOR_MAIN_FIGHT);
  else
    move_screen_cursor(battle_menu.screen_cursor + 1);
}

/**
 * Called when the user presses 'A' on the main menu.
 */
static void main_menu_cursor_commit(void) {
  switch (battle_menu.screen_cursor) {
  case BATTLE_CURSOR_MAIN_FIGHT:
    open_battle_menu(BATTLE_MENU_FIGHT);
    break;
  case BATTLE_CURSOR_MAIN_ABILITY:
    open_battle_menu(BATTLE_MENU_ABILITY);
    break;
  case BATTLE_CURSOR_MAIN_ITEM:
    open_battle_menu(BATTLE_MENU_ITEM);
    break;
  case BATTLE_CURSOR_MAIN_FLEE:
    confirm_flee();
    break;
  }
}

/**
 * Performs game logic for the battle menu.
 * @see `update_battle`
 */
static inline void update_battle_menu(void) {
  switch (battle_menu.active_menu) {
  case BATTLE_MENU_MAIN:
    if (was_pressed(J_A))
      main_menu_cursor_commit();
    else if (on_dpad(J_UP))
      main_menu_cursor_up();
    else if (on_dpad(J_DOWN))
      main_menu_cursor_down();
    break;
  case BATTLE_MENU_FIGHT:
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    else if (was_pressed(J_A))
      confirm_fight();
    else if (on_dpad(J_LEFT))
      select_prev_enemy();
    else if (on_dpad(J_RIGHT))
      select_next_enemy();
    break;
  case BATTLE_ABILITY_MONSTER_SELECT:
    if (was_pressed(J_B)) {
      battle_menu.active_menu = BATTLE_MENU_ABILITY;
      move_screen_cursor(battle_menu.last_ability_cursor);
    } else if (was_pressed(J_A))
      confirm_ability(battle_menu.active_ability);
    else if (was_pressed(J_LEFT))
      select_prev_enemy();
    else if (was_pressed(J_RIGHT))
      select_next_enemy();
    break;
  case BATTLE_MENU_ABILITY:
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    else if (was_pressed(J_A) && battle_menu.entries > 0) {
      const Ability *ability = player_abilities[battle_menu.cursor];

      if (player.sp < ability->sp_cost) {
        play_sound(sfx_error);
        break;
      }

      if (ability->target_type == TARGET_SINGLE) {
        battle_menu.active_ability = ability;
        battle_menu.last_ability_cursor = battle_menu.screen_cursor;
        open_battle_menu(BATTLE_ABILITY_MONSTER_SELECT);
      } else {
        confirm_ability(ability);
      }
    }
    else if (on_dpad(J_UP))
      submenu_cursor_up();
    else if (on_dpad(J_DOWN))
      submenu_cursor_down();
    break;
  case BATTLE_MENU_ITEM:
    // Select an item from the inventory
    if (was_pressed(J_B))
      open_battle_menu(BATTLE_MENU_MAIN);
    else if (was_pressed(J_A)) {
      ItemId item_id = battle_menu.item_at[battle_menu.cursor];

      if (!can_use_item(item_id)) {
        play_sound(sfx_error);
        break;
      }

      if (!remove_item(item_id))
        remove_item_text_at_cursor();
      else
        update_item_text_at_cursor();

      confirm_item(item_id);
    }
    else if (on_dpad(J_UP))
      submenu_cursor_up();
    else if (on_dpad(J_DOWN))
      submenu_cursor_down();
    break;
  }
}

static inline uint16_t tween_hp(uint16_t hp, uint16_t target, int16_t delta) {
  if (hp == target)
    return target;

  if (delta < 0) {
    delta = (-delta) >> ANIMATION_HP_DELTA_FACTOR;
    if (delta == 0)
      delta = 1;
    if (hp < target + delta)
      return target;
    return hp - delta;
  }

  delta >>= ANIMATION_HP_DELTA_FACTOR;
  if (delta == 0)
    delta = 1;
  if (hp + delta > target)
    return target;
  return hp + delta;
}

static inline bool animate_monster_hp_bars(void) {
  bool updated = false;
  Monster *monster = encounter.monsters;
  for (uint8_t pos = 0; pos < 3; pos++, monster++) {
    if (!monster->active)
      continue;
    if (monster->hp == monster->target_hp)
      continue;
    updated = true;
    monster->hp = tween_hp(monster->hp, monster->target_hp, monster->hp_delta);
    draw_hp_bar(pos, monster->hp, monster->max_hp);
  }
  return updated;
}

static inline void reset_monster_death_animation(void) {
  monster_death_step = 0;
  monster_death_state = MONSTER_DEATH_START;
}

/**
 * Palette animation for dying monsters.
 */
static inline bool animate_monster_death(void) {
  Monster *monster = encounter.monsters;

  if (monster_death_state == MONSTER_DEATH_DONE)
    return false;

  if (monster_death_state == MONSTER_DEATH_START) {
    for (uint8_t pos = 0; pos < 3; pos++, monster++) {
      if (monster->fled) {
        monster_death_state = MONSTER_DEATH_ANIMATE;
        return true;
      }

      if (!monster->active)
        continue;

      if (monster->hp == 0) {
        monster_death_state = MONSTER_DEATH_ANIMATE;
        play_sound(sfx_monster_death);
        return true;
      }
    }
    monster_death_state = MONSTER_DEATH_DONE;
    return false;
  }

  if (monster_death_step == 0) {
    init_timer(monster_death_timer, MONSTER_DEATH_INITIAL_DELAY);
  } else {
    if (!update_timer(monster_death_timer))
      return true;
    init_timer(monster_death_timer, MONSTER_DEATH_FADE_DELAY);
  }

  for (uint8_t pos = 0; pos < 3; pos++, monster++) {
    if (!monster->active)
      continue;
    if (monster->hp != 0 && !monster->fled)
      continue;
    if (monster_death_step == 0)
      toggle_hp_bar_palette(pos);
    set_bkg_palette(pos + 1, 1, monster_death_colors + 4 * monster_death_step);
  }

  monster_death_step++;
  if (monster_death_step > 5){
    monster_death_state = MONSTER_DEATH_DONE;
  }

  return true;
}


/**
 * Update handler that animates the results of player & monster actions.
 */
static void animate_action_result(void) {
  if (is_screen_shaking) {
    if (update_timer(screen_shake_timer)) {
      reset_timer(screen_shake_timer);
      const int8_t shake = *(screen_shake + screen_shake_index);
      SCX_REG = shake;
      screen_shake_index++;
      if (shake == 0) {
        is_screen_shaking = false;
      }
    }
  }

  switch (animation_state) {
  case ANIMATION_PREAMBLE:
    if (skip_post_message && battle_sfx && update_timer(no_post_sfx_timer)) {
      play_sound(battle_sfx);
      battle_sfx = NULL;
    }

    if (text_writer_done()) {
      init_timer(effect_delay_timer, 24);
      animation_state = ANIMATION_EFFECT;
      if (battle_sfx) {
        play_sound(battle_sfx);
        battle_sfx = NULL;
      }
      if (player_was_hit) {
        player_was_hit = false;
        is_screen_shaking = true;
        screen_shake_index = 0;
        init_timer(screen_shake_timer, 3);
      }
    }
    break;
  case ANIMATION_EFFECT:
    if (update_timer(effect_delay_timer)) {
      if (skip_post_message)
        skip_post_message = false;
      else
        text_writer.print(battle_post_message);
      reset_monster_death_animation();
      animation_state = ANIMATION_RESULT;
    }
    break;
  case ANIMATION_RESULT:
    bool graphics_updated = false;


    // If the player fled, play the "footsteps" sound
    if (encounter.player_fled && !flee_sound_played) {
      flee_sound_played = true;
      play_sound(sfx_stairs);
    }

    // Sequence action result animations
    if (animate_monster_hp_bars()) {
      graphics_updated = true;
    } else if (animate_monster_death()) {
      graphics_updated = true;
    }

    // Done when the battle text is finished an no more updates are performed.
    if (!graphics_updated && text_writer_done()) {
      animation_state = ANIMATION_COMPLETE;
    }
    break;
  }
}

/**
 * Checks for status effect changes and updates UI accordingly.
 */
static inline void update_status_effects_ui(void) {
  redraw_player_status_effects();
  redraw_monster_status_effects();
}

/**
 * Clears inactive monster palettes / graphics.
 */
static void clear_inactive_monsters(void) {
  Monster *monster = encounter.monsters;
  for (uint8_t pos = 0; pos < 3; pos++, monster++)
    if (!monster->active)
      core.load_bg_palette(blank_palette, pos + 1, 1);
}

/**
 * LCY interrupt handler for the fight and skill menus. This handler changes the
 * scroll-y position at a specific scanline to display a different part of the
 * background that contains the graphics for these menus.
 */
static void fight_menu_isr(void) {
  SCY_REG = 0;
  if (
    battle_state == BATTLE_STATE_MENU &&
    battle_menu.active_menu != BATTLE_MENU_MAIN &&
    battle_menu.active_menu != BATTLE_MENU_FIGHT
  ) {
    SCY_REG = 64;
    return;
  }
}

/**
 * Initializes the battle system based on the current encounter.
 */
void initialize_battle(void) {
  DISPLAY_OFF;

  move_bkg(0, 0);

  // Reset the background, window, and sprites
  core.fill_bg(BATTLE_CLEAR_TILE, BATTLE_CLEAR_ATTR);
  hide_sprites();
  hide_window();

  // Load palettes & fonts
  core.load_bg_palette(data_battle_bg_colors, 0, 8);
  core.load_sprite_palette(data_battle_sprite_colors, 0, 8);
  core.load_font();
  core.load_battle_tiles();

  // Configure the text writer
  text_writer.set_origin(VRAM_WINDOW, 1, 1);
  text_writer.set_auto_page(player.message_speed);
  text_writer.set_size(18, 4);

  // Memoize some stuff for fast rendering
  status_effect_x[0] = get_status_effect_x(MONSTER_POSITION1);
  status_effect_x[1] = get_status_effect_x(MONSTER_POSITION2);
  status_effect_x[2] = get_status_effect_x(MONSTER_POSITION3);

  // Draw menus and initialize cursor
  core.draw_tilemap(battle_menu_tilemap, VRAM_BACKGROUND_XY(0, MENU_Y));
  core.draw_tilemap(battle_submenu_tilemap, VRAM_BACKGROUND_XY(0, SUBMENU_Y));
  core.draw_tilemap(battle_textbox_tilemap, VRAM_WINDOW_XY(0, 0));

  for (uint8_t s = 0; s < 4; s++) {
    set_sprite_tile(CURSOR_SPRITE + s, CURSOR_TILE + s);
    set_sprite_prop(CURSOR_SPRITE + s, CURSOR_ATTR);
  }
  move_screen_cursor_no_sound(BATTLE_CURSOR_MAIN_FIGHT);
  battle_menu.active_menu = BATTLE_MENU_MAIN;

  // Initialize the encounter and player graphics
  battle_init_encounter();
  flee_sound_played = false;

  // Update the player UI
  set_magic_or_martial();
  update_player_hp();
  update_player_mp();
  render_ability_text();
  render_item_text();

  // Attach an LCY=LY interrupt to handle the menu display.
  CRITICAL {
    LYC_REG = 90;
    STAT_REG = STATF_LYC;
    add_LCD(fight_menu_isr);
    add_LCD(nowait_int_handler);
  }
  set_interrupts(IE_REG | LCD_IFLAG);

  // Initiate the fade-in and start battle
  battle_state = BATTLE_FADE_IN;
  fade_in();
  game_state = GAME_STATE_BATTLE;
  player_died = false;

  DISPLAY_ON;
}

void init_battle(void) NONBANKED {
  // Switch to the battle ROM bank (has stats tables, etc.)
  SWITCH_ROM(BATTLE_ROM_BANK);
  initialize_battle();
}

/**
 * Leaves battle and returns to the map system.
 */
static void leave_battle(void) {
  if (encounter.is_test) {
    battle_state = BATTLE_INACTIVE;
    return;
  }

  fade_out();
  toggle_sprites();
  battle_state = BATTLE_COMPLETE;
}

static void cleanup_isr(void) {
  CRITICAL {
    remove_LCD(fight_menu_isr);
    remove_LCD(nowait_int_handler);
    STAT_REG = 0;
  }
  set_interrupts(IE_REG & ~LCD_IFLAG);
}

void update_battle(void) NONBANKED {
  if (battle_state == BATTLE_COMPLETE) {
    // Important, don't remove
    return;
  }

  switch (battle_state) {
  case BATTLE_FADE_IN:
    // Important, keep
    return;
  case BATTLE_STATE_MENU:
    update_battle_menu();
    break;
  case BATTLE_ROLL_INITIATIVE:
    before_round();
    roll_initiative();
    hide_cursor();
    text_writer.clear();
    show_battle_text();
    battle_state = BATTLE_NEXT_TURN;
    break;
  case BATTLE_NEXT_TURN:
    if (encounter.player_died) {
      play_sound(sfx_battle_death);
      sprintf(rewards_buf, "You died.");
      text_writer.print(rewards_buf);
      battle_state = BATTLE_PLAYER_DIED;
      return;
    }

    if (encounter.victory) {
      apply_rewards();
      battle_state = BATTLE_REWARDS;
      return;
    }

    if (encounter.player_fled) {
      init_timer(effect_delay_timer, FLEE_DELAY_FRAMES);
      battle_state = BATTLE_PLAYER_FLED;
      return;
    }

    next_turn();

    if (encounter.round_complete) {
      battle_state = BATTLE_END_ROUND;
    } else {
      check_status_effects();
      battle_state = BATTLE_TAKE_ACTION;
    }
    break;
  case BATTLE_TAKE_ACTION:
    take_action();
    text_writer.print(battle_pre_message);
    animation_state = ANIMATION_PREAMBLE;
    if (skip_post_message)
      init_timer(no_post_sfx_timer, 6);
    battle_state = BATTLE_ANIMATE;
    break;
  case BATTLE_ACTION_CLEANUP:
    after_action();
    clear_inactive_monsters();
    battle_state = BATTLE_UI_UPDATE;
    break;
  case BATTLE_PLAYER_FLED:
    if (!update_timer(effect_delay_timer))
      return;
    leave_battle();
    break;
  case BATTLE_PLAYER_DIED:
    // Transition to the game over screen.
    player_died = true;
    text_writer.update();
    if (text_writer_done()) {
      init_timer(death_delay_timer, 90);
      battle_state = BATTLE_DIED_DELAY;
    }
    break;
  case BATTLE_DIED_DELAY:
    if (!update_timer(death_delay_timer))
      return;
    leave_battle();
    break;
  case BATTLE_END_ROUND:
    battle_state = BATTLE_STATE_MENU;
    battle_menu.active_menu = BATTLE_MENU_MAIN;
    update_player_mp();
    hide_battle_text();
    play_sound(sfx_next_round);
    move_screen_cursor_no_sound(BATTLE_CURSOR_MAIN_FIGHT);
    break;
  case BATTLE_REWARDS:
    text_writer.auto_page = AUTO_PAGE_OFF;
    text_writer.print(rewards_buf);
    battle_state = BATTLE_SUCCESS;
    break;
  case BATTLE_SUCCESS:
    if (!was_pressed(J_A))
      break;
    if (text_writer.state == TEXT_WRITER_PAGE_WAIT)
      text_writer.next_page();
    else if (text_writer.state == TEXT_WRITER_DONE)
      leave_battle();
    break;
  case BATTLE_LEVEL_UP:
    // Handle level up messages and logic.
    break;
  }
}

void draw_battle(void) NONBANKED {
  switch (battle_state) {
  case BATTLE_SUCCESS:
    text_writer.update();
    break;
  case BATTLE_ANIMATE:
    text_writer.update();
    animate_action_result();
    if (animation_state == ANIMATION_COMPLETE)
      battle_state = BATTLE_ACTION_CLEANUP;
    break;
  case BATTLE_UI_UPDATE:
    update_status_effects_ui();
    update_player_hp();
    battle_state = BATTLE_NEXT_TURN;
    break;
  case BATTLE_COMPLETE:
    if (fade_update()) {
      // Return to the world map.
      DISPLAY_OFF;
      cleanup_isr();
      battle_state = BATTLE_INACTIVE;

      if (player_died) {
        player_died = false;
        return_from_death();
      } else if (encounter.is_final_boss) {
        init_credits();
      } else {
        return_from_battle();
      }
      return;
    }
    break;
  case BATTLE_FADE_IN:
    if (fade_update()) {
      battle_state = BATTLE_STATE_MENU;
      toggle_sprites();
    }
    break;
  }
  if (!is_screen_shaking)
    move_bkg(0, 0);
}
