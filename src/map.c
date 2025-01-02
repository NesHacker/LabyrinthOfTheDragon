#pragma bank 2

#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "battle.h"
#include "core.h"
#include "floor.h"
#include "map.h"
#include "sound.h"

MapSystem map = { MAP_STATE_WAITING };

/**
 * Whether or not to initiate the wall hit sound effect.
 */
static bool play_wall_hit = true;

/**
 * Map tile data for the tile the hero currently occupies and those in every
 * cardinal direction (index this with a `Direction`).
 */
static MapTile local_tiles[5];

/**
 * Stores a buffer of tiles to be progressively loaded over the animation frames
 * of a move.
 */
static MapTile tile_buf[2 * MAP_HORIZ_LOADS];

/**
 * Map game object hashtable. Makes it easy to lookup game objects based on the
 * map and tile position in the map.
 */
static TileHashEntry tile_object_hashtable[TILE_HASHTABLE_SIZE];

/**
 * Holds the color of the flames for the sconces in the current floor.
 */
FlameColor sconce_colors[8] = {
  FLAME_RED, FLAME_RED, FLAME_RED, FLAME_RED,
  FLAME_RED, FLAME_RED, FLAME_RED, FLAME_RED,
};

/**
 * Hash function for positions in floor maps.
 * @param map_id Id of the map for the position.
 * @param x X position in the map.
 * @param y Y position in the map.
 */
static uint8_t hash(uint8_t map_id, int8_t x, int8_t y) {
  uint8_t k = map_id;
  k ^= ((uint8_t)x << 1);
  k ^= ((uint8_t)y << 2);
  return k & (TILE_HASHTABLE_SIZE - 1);
}

/**
 * Adds an entry for the given object to the tile objects hashtable.
 * @param type Type of object being added.
 * @param object Pointer to the object.
 * @param map_id Map on which the object resides.
 * @param x Horizontal position for the object.
 * @param y Vertical position for the object.
 */
static void hash_object(
  TileHashType type,
  void *object,
  uint8_t map_id,
  int8_t x,
  int8_t y
) {
  TileHashEntry *entry = tile_object_hashtable + hash(map_id, x, y);

  if (entry->data) {
    while (entry->next) {
      entry = entry->next;
    }

    TileHashEntry *next = (TileHashEntry *)malloc(sizeof(TileHashEntry));
    entry->next = next;
    entry = next;
    entry->next = NULL;
  }

  entry->map_id = map_id;
  entry->x = x;
  entry->y = y;
  entry->type = type;
  entry->data = object;
}

/**
 * Gets a hash entry at the given coordinates in the active map.
 */
static TileHashEntry *get_hash_entry(int8_t x, int8_t y) {
  const uint8_t map_id = map.active_map->id;
  const uint8_t hash_idx = hash(map_id, x, y);
  TileHashEntry *entry = tile_object_hashtable + hash_idx;

  while (entry && entry->data) {
    if (entry->map_id == map_id && entry->x == x && entry->y == y)
      return entry;
    entry = entry->next;
  }

  return NULL;
}

/**
 * Resets the object hash and frees allocated bucket items.
 */
static void reset_object_hash(void) {
  TileHashEntry *entry = tile_object_hashtable;

  for (uint8_t k = 0; k < TILE_HASHTABLE_SIZE; k++, entry++) {
    entry->data = NULL;
    entry->map_id = 0xFF;
    entry->x = (int8_t)0xFF;
    entry->y = (int8_t)0xFF;

    TileHashEntry *sibling = entry->next;
    entry->next = NULL;

    while (sibling) {
      TileHashEntry *tmp = sibling;
      free(tmp);
      sibling = sibling->next;
    }
  }
}

/**
 * Initializes the hero character sprites.
 */
static void init_hero(void) {
  const uint8_t hero_x = 16 * HERO_X_OFFSET;
  const uint8_t hero_y = 16 * HERO_Y_OFFSET;
  const uint8_t offset = 0x00;
  const uint8_t frame = (map.walk_frame << 1) + offset;

  init_timer(map.walk_timer, 8);

  set_sprite_tile(0, frame);
  set_sprite_tile(1, frame + 0x01);
  set_sprite_tile(2, frame + 0x10);
  set_sprite_tile(3, frame + 0x11);

  set_sprite_prop(0, 0);
  set_sprite_prop(1, 0);
  set_sprite_prop(2, 0);
  set_sprite_prop(3, 0);

  move_sprite(0, hero_x + 8, hero_y + 16);
  move_sprite(1, hero_x + 8 + 8, hero_y + 16);
  move_sprite(2, hero_x + 8, hero_y + 8 + 16);
  move_sprite(3, hero_x + 8 + 8, hero_y + 8 + 16);

  PlayerClass pc = (player.player_class == CLASS_TEST) ?
    CLASS_DRUID :
    player.player_class;
  core.load_hero_tiles(pc);
  core.load_sprite_palette(hero_colors + 4 * pc, 0, 1);
}

/**
 * Updates the hero sprites each frame.
 */
static void update_hero(void) {
  const uint8_t hero_x = 16 * HERO_X_OFFSET;
  const uint8_t hero_y = 16 * HERO_Y_OFFSET;
  if (map.hero_state == HERO_WALKING) {
    if (update_timer(map.walk_timer)) {
      reset_timer(map.walk_timer);
      map.walk_frame ^= 1;
    }
  } else {
    map.walk_frame = 0;
  }

  uint8_t offset = 0x00;
  switch (map.hero_direction) {
  case DOWN:
    offset = 0x00;
    break;
  case UP:
    offset = 0x04;
    break;
  case LEFT:
  case RIGHT:
    offset = 0x08;
    break;
  }

  const uint8_t frame = (map.walk_frame << 1) + offset;

  if (map.hero_direction != LEFT) {
    set_sprite_tile(0, frame);
    set_sprite_tile(1, frame + 0x01);
    set_sprite_tile(2, frame + 0x10);
    set_sprite_tile(3, frame + 0x11);
    set_sprite_prop(0, 0b00000000);
    set_sprite_prop(1, 0b00000000);
    set_sprite_prop(2, 0b00000000);
    set_sprite_prop(3, 0b00000000);
  } else {
    set_sprite_tile(1, frame);
    set_sprite_tile(0, frame + 0x01);
    set_sprite_tile(3, frame + 0x10);
    set_sprite_tile(2, frame + 0x11);
    set_sprite_prop(0, 0b00100000);
    set_sprite_prop(1, 0b00100000);
    set_sprite_prop(2, 0b00100000);
    set_sprite_prop(3, 0b00100000);
  }

  move_sprite(0, hero_x + 8, hero_y + 16);
  move_sprite(1, hero_x + 8 + 8, hero_y + 16);
  move_sprite(2, hero_x + 8, hero_y + 8 + 16);
  move_sprite(3, hero_x + 8 + 8, hero_y + 8 + 16);
}

/**
 * Clears hero sprites.
 */
static void clear_hero(void) {
  move_sprite(0, 0, 0);
  move_sprite(1, 0, 0);
  move_sprite(2, 0, 0);
  move_sprite(3, 0, 0);
}

/**
 * Clears NPC sprites.
 */
static void clear_npcs(void) {
  for (uint8_t k = NPC_SPRITE_1; k < NPC_SPRITE_2 + 4; k++)
    move_sprite(k, 0, 0);
}

/**
 * @return Monster tiles.
 * @param type Type of monter for which to find the tiles.
 */
static MonsterTiles monster_tiles_by_type(MonsterType type) {
  switch (type) {
  case MONSTER_GOBLIN:
    return MONSTER_TILES_GOBLIN;
  case MONSTER_ZOMBIE:
    return MONSTER_TILES_ZOMBIE;
  case MONSTER_BUGBEAR:
    return MONSTER_TILES_BUGBEAR;
  case MONSTER_OWLBEAR:
    return MONSTER_TILES_OWLBEAR;
  case MONSTER_GELATINOUS_CUBE:
    return MONSTER_TILES_GELATINOUS_CUBE;
  case MONSTER_DISPLACER_BEAST:
    return MONSTER_TILES_DISPLACER_BEAST;
  case MONSTER_WILL_O_WISP:
    return MONSTER_TILES_WILL_O_WISP;
  case MONSTER_DEATHKNIGHT:
    return MONSTER_TILES_DEATHKNIGHT;
  case MONSTER_MINDFLAYER:
    return MONSTER_TILES_MINDFLAYER;
  case MONSTER_BEHOLDER:
    return MONSTER_TILES_BEHOLDER;
  case MONSTER_DRAGON:
    return MONSTER_TILES_DRAGON;
  default:
    return MONSTER_TILES_KOBOLD;
  }
}

/**
 * @return Palette colors for the monster.
 * @param type Type of monster for which to find the palette colors.
 */
static palette_color_t *monster_palette_by_type(MonsterType type) {
  return kobold_palettes;
}

/**
 * Initializes NPC sprites.
 */
static void init_npcs(void) {
  init_timer(map.npc_walk_timer, 16);

  MonsterTilePosition pos = MONSTER_TILE_POSITION1;
  const NPC *npc;
  for (
    npc = map.active_floor->npcs;
    pos <= MONSTER_TILE_POSITION2 && npc->id != END;
    npc++, pos++
  ) {
    MonsterTiles monster_tiles = monster_tiles_by_type(npc->monster_type);
    palette_color_t *palette = monster_palette_by_type(npc->monster_type);

    const uint8_t palette_num = pos + 6;
    core.load_monster_tiles(monster_tiles, pos);
    core.load_sprite_palette(kobold_palettes, palette_num, 1);

    const uint8_t sprite_root = NPC_SPRITE_1 + 4 * pos;
    const uint8_t tile_root = NPC_1_TILE_ROOT + 0x20 * pos;
    set_sprite_tile(sprite_root, tile_root);
    set_sprite_tile(sprite_root + 1, tile_root + 1);
    set_sprite_tile(sprite_root + 2, tile_root + 0x10);
    set_sprite_tile(sprite_root + 3, tile_root + 0x10 + 1);
    set_sprite_prop(sprite_root + 0, NPC_BASE_PROP | palette_num);
    set_sprite_prop(sprite_root + 1, NPC_BASE_PROP | palette_num);
    set_sprite_prop(sprite_root + 2, NPC_BASE_PROP | palette_num);
    set_sprite_prop(sprite_root + 3, NPC_BASE_PROP | palette_num);
    move_sprite(sprite_root + 0, 0, 0);
    move_sprite(sprite_root + 1, 0, 0);
    move_sprite(sprite_root + 2, 0, 0);
    move_sprite(sprite_root + 3, 0, 0);
  }
}

/**
 * Updates NPCs.
 */
static void update_npcs(void) {
  if (update_timer(map.npc_walk_timer)) {
    reset_timer(map.npc_walk_timer);
    map.npc_walk_frame ^= 2;
  }

  MonsterTilePosition pos = MONSTER_TILE_POSITION1;
  const NPC *npc;
  for (
    npc = map.active_floor->npcs;
    pos <= MONSTER_TILE_POSITION2 && npc->id != END;
    npc++, pos++
  ) {
    const uint8_t sprite_root = NPC_SPRITE_1 + 4 * pos;

    *(debug + 1) = *(debug + 1) + 1;

    if (
      is_npc_visible(npc->id) &&
      npc->map_id == map.active_map->id &&
      npc->col >= map.x - 1 &&
      npc->col < map.x + MAP_HORIZ_LOADS &&
      npc->row >= map.y - 1 &&
      npc->row < map.y + MAP_VERT_LOADS
    ) {
      const uint8_t tile_root =
        NPC_1_TILE_ROOT + 0x20 * pos + map.npc_walk_frame;

      uint8_t x = ((npc->col - map.x + 1) << 4) - 8;
      uint8_t y = ((npc->row - map.y + 1) << 4);

      if (map.state == MAP_STATE_MOVING) {
        switch (map.move_direction) {
        case UP:
          y += map.move_step - 15;
          break;
        case DOWN:
          y -= map.move_step - 15;
          break;
        case LEFT:
          x += map.move_step - 15;
          break;
        case RIGHT:
          x -= map.move_step - 15;
          break;
        }
      }

      move_sprite(sprite_root + 0, x, y);
      move_sprite(sprite_root + 1, x + 8, y);
      move_sprite(sprite_root + 2, x, y + 8);
      move_sprite(sprite_root + 3, x + 8, y + 8);

      set_sprite_tile(sprite_root, tile_root);
      set_sprite_tile(sprite_root + 1, tile_root + 1);
      set_sprite_tile(sprite_root + 2, tile_root + 0x10);
      set_sprite_tile(sprite_root + 3, tile_root + 0x10 + 1);

    } else {
      move_sprite(sprite_root + 0, 0, 0);
      move_sprite(sprite_root + 1, 0, 0);
      move_sprite(sprite_root + 2, 0, 0);
      move_sprite(sprite_root + 3, 0, 0);
    }
  }
}

/**
 * Palettes used for flames.
 */
const palette_color_t flame_palettes[] = {
  // Red flame
  RGB_BLACK,
  RGB8(213, 200, 89),
  RGB8(132, 88, 32),
  RGB8(72, 21, 13),
  // Green flame
  RGB_BLACK,
  RGB8(89, 213, 200),
  RGB8(32, 132, 88),
  RGB8(13, 72, 21),
  // Blue flame
  RGB_BLACK,
  RGB8(200, 89, 213),
  RGB8(88, 32, 132),
  RGB8(21, 13, 72),
};

const palette_color_t torch_gauge_palettes[] = {
  // Extinguished
  RGB_BLACK,
  RGB8(80, 80, 80),
  RGB8(140, 140, 140),
  RGB8(200, 200, 200),
  // Red Flame
  RGB_BLACK,
  RGB8(72, 21, 13),
  RGB8(132, 88, 32),
  RGB8(213, 200, 89),
  // Green Flame
  RGB_BLACK,
  RGB8(13, 72, 21),
  RGB8(32, 132, 88),
  RGB8(89, 213, 200),
  // Blue Flame
  RGB_BLACK,
  RGB8(21, 13, 72),
  RGB8(88, 32, 132),
  RGB8(200, 89, 213),
};

const palette_color_t magic_keys_palette[] = {
  RGB_BLACK,
  RGB8(88, 32, 132),
  RGB8(200, 89, 213),
  RGB8(240, 240, 240),
};

/**
 * @return The flame sprite id for the given sconce.
 * @param s The id of the sconce.
 */
static uint8_t get_sconce_flame_sprite(SconceId s) {
  switch (s) {
  case SCONCE_1: return FLAME_1;
  case SCONCE_2: return FLAME_2;
  case SCONCE_3: return FLAME_3;
  case SCONCE_4: return FLAME_4;
  case SCONCE_5: return FLAME_5;
  case SCONCE_6: return FLAME_6;
  case SCONCE_7: return FLAME_7;
  default: return FLAME_8;
  }
}

/**
 * @return Index for the sconce with the given id.
 * @param id Id of the sconce.
 */
static uint8_t get_sconce_index(SconceId s) {
  switch (s) {
  case SCONCE_1: return 0;
  case SCONCE_2: return 1;
  case SCONCE_3: return 2;
  case SCONCE_4: return 3;
  case SCONCE_5: return 4;
  case SCONCE_6: return 5;
  case SCONCE_7: return 6;
  default: return 7;
  }
}

/**
 * Initializes sconce flame sprites.
 */
static void init_flames(void) {
  init_timer(map.flame_timer, 20);
  map.flame_frame = 0;
  core.load_sprite_palette(flame_palettes, 1, 3);

  for (uint8_t k = FLAME_1; k <= FLAME_8; k++) {
    set_sprite_tile(k, FLAME_TILE_1);
    set_sprite_prop(k, 0b00001001);
    move_sprite(k, 0, 0);
  }

  const Sconce *sconce;
  for (sconce = map.active_floor->sconces; sconce->id != END; sconce++) {
    const uint8_t sprite_id = get_sconce_flame_sprite(sconce->id);

    if (sconce->is_lit || sconce->id == SCONCE_STATIC)
      sconce_colors[get_sconce_index(sconce->id)] = sconce->color;

    switch (sconce->color) {
    case FLAME_RED_PROP:
      set_sprite_prop(sprite_id, FLAME_RED_PROP);
      break;
    case FLAME_GREEN:
      set_sprite_prop(sprite_id, FLAME_GREEN_PROP);
      break;
    case FLAME_BLUE:
      set_sprite_prop(sprite_id, FLAME_BLUE_PROP);
      break;
    default:
      set_sprite_prop(sprite_id, FLAME_RED_PROP);
    }
  }
}

/**
 * Updates flame sprites and animation.
 */
static void update_flames(void) {
  if (update_timer(map.flame_timer)) {
    reset_timer(map.flame_timer);
    map.flame_frame ^= 1;
    const uint8_t tile = map.flame_frame ? FLAME_TILE_2 : FLAME_TILE_1;
    for (uint8_t k = FLAME_1; k <= FLAME_8; k++) {
      set_sprite_tile(k, tile);
    }
  }

  uint8_t flame_id = 0;

  const Sconce *sconce;
  for (sconce = map.active_floor->sconces; sconce->id != END; sconce++) {
    const uint8_t sprite_id = FLAME_SPRITE_ID0 + flame_id; //get_sconce_flame_sprite(sconce->id);

    if (
      is_sconce_lit(sconce->id) &&
      sconce->map_id == map.active_map->id &&
      sconce->col >= map.x - 1 &&
      sconce->col < map.x + MAP_HORIZ_LOADS &&
      sconce->row >= map.y - 1 &&
      sconce->row < map.y + MAP_VERT_LOADS
    ) {
      flame_id++;

      // Sconce is lit and on the active screen.
      uint8_t x = ((sconce->col - map.x + 1) << 4) - 4;
      uint8_t y = ((sconce->row - map.y + 1) << 4) + 2;

      if (map.state == MAP_STATE_MOVING) {
        switch (map.move_direction) {
        case UP:
          y += map.move_step - 15;
          break;
        case DOWN:
          y -= map.move_step - 15;
          break;
        case LEFT:
          x += map.move_step - 15;
          break;
        case RIGHT:
          x -= map.move_step - 15;
          break;
        }
      }

      FlameColor color = sconce->id == SCONCE_STATIC ?
        sconce->color :
        sconce_colors[get_sconce_index(sconce->id)];
      move_sprite(sprite_id, x, y);
      set_sprite_prop(sprite_id, FLAME_SPRITE_PROP | color);
    } else {
      // Sconce is offscreen
      move_sprite(sprite_id, 0, 0);
    }
  }

  for (; flame_id < MAX_FLAME_SPRITES; flame_id++) {
    move_sprite(flame_id + FLAME_SPRITE_ID0, 0, 0);
  }
}

/**
 * Clears all flame sprites.
 */
static void clear_flames(void) {
  for (uint8_t k = FLAME_1; k <= FLAME_8; k++)
    move_sprite(k, 0, 0);
}

/**
 * Initializes the player hud (torch gauge, keys, floor, etc.).
 */
static void init_hud(void) {
  const palette_color_t *tgp = torch_gauge_palettes + player.torch_color * 4;
  core.load_sprite_palette(tgp, TORCH_GAUGE_PALETTE, 1);
  core.load_sprite_palette(magic_keys_palette, MAGIC_KEY_HUD_PALETTE, 1);

  init_timer(map.torch_timer, TORCH_GAUGE_SPEED);

  set_sprite_tile(TORCH_GAUGE_FLAME, FLAME_TILE_1);
  set_sprite_prop(TORCH_GAUGE_FLAME, TORCH_GAUGE_PROP);

  if (player.has_torch)
    move_sprite(TORCH_GAUGE_FLAME, TORCH_GAUGE_X, TORCH_GAUGE_Y);
  else
    move_sprite(TORCH_GAUGE_FLAME, 0, 0);

  for (uint8_t k = TORCH_GAUGE_BODY_1; k <= TORCH_GAUGE_BODY_4; k++) {
    set_sprite_tile(k, TORCH_GAUGE_ZERO);
    set_sprite_prop(k, TORCH_GAUGE_PROP);

    if (!player.has_torch) {
      move_sprite(k, 0, 0);
      continue;
    }

    switch (k) {
    case TORCH_GAUGE_BODY_1:
      move_sprite(k, TORCH_GAUGE_X + 10, TORCH_GAUGE_Y);
      break;
    case TORCH_GAUGE_BODY_2:
      move_sprite(k, TORCH_GAUGE_X + 18, TORCH_GAUGE_Y);
      break;
    case TORCH_GAUGE_BODY_3:
      move_sprite(k, TORCH_GAUGE_X + 26, TORCH_GAUGE_Y);
      break;
    case TORCH_GAUGE_BODY_4:
      move_sprite(k, TORCH_GAUGE_X + 34, TORCH_GAUGE_Y);
      break;
    }
  }

  set_sprite_tile(MAGIC_KEY_SPRITE_1, MAGIC_KEY_TOP_TILE);
  set_sprite_tile(MAGIC_KEY_SPRITE_2, MAGIC_KEY_BOTTOM_TILE);
  set_sprite_tile(MAGIC_KEY_QTY, MAGIC_KEY_NUM_0);

  set_sprite_prop(MAGIC_KEY_SPRITE_1, MAGIC_KEY_HUD_ATTR);
  set_sprite_prop(MAGIC_KEY_SPRITE_2, MAGIC_KEY_HUD_ATTR);
  set_sprite_prop(MAGIC_KEY_QTY, MAGIC_KEY_HUD_ATTR);

  if (player.got_magic_key) {
    move_sprite(MAGIC_KEY_SPRITE_1, MAGIC_KEYS_X, MAGIC_KEYS_Y - 1);
    move_sprite(MAGIC_KEY_SPRITE_2, MAGIC_KEYS_X, MAGIC_KEYS_Y + 7);
    move_sprite(MAGIC_KEY_QTY, MAGIC_KEYS_X + 9, MAGIC_KEYS_Y);
  } else {
    move_sprite(MAGIC_KEY_SPRITE_1, 0, 0);
    move_sprite(MAGIC_KEY_SPRITE_2, 0, 0);
    move_sprite(MAGIC_KEY_QTY, 0, 0);
  }
}

/**
 * Updates the player hud.
 */
static void update_hud(void) {
  if (player.has_torch) {
    if (player.torch_gauge > 0) {
      set_sprite_tile(TORCH_GAUGE_FLAME,
        map.flame_frame ? FLAME_TILE_1 : FLAME_TILE_2);

      if (player.torch_gauge > 24) {
        const uint8_t t = TORCH_GAUGE_ZERO + (player.torch_gauge - 24);
        set_sprite_tile(TORCH_GAUGE_BODY_1, TORCH_GAUGE_ZERO + 8);
        set_sprite_tile(TORCH_GAUGE_BODY_2, TORCH_GAUGE_ZERO + 8);
        set_sprite_tile(TORCH_GAUGE_BODY_3, TORCH_GAUGE_ZERO + 8);
        set_sprite_tile(TORCH_GAUGE_BODY_4, t);
      } else if (player.torch_gauge > 16) {
        const uint8_t t = TORCH_GAUGE_ZERO + (player.torch_gauge - 16);
        set_sprite_tile(TORCH_GAUGE_BODY_1, TORCH_GAUGE_ZERO + 8);
        set_sprite_tile(TORCH_GAUGE_BODY_2, TORCH_GAUGE_ZERO + 8);
        set_sprite_tile(TORCH_GAUGE_BODY_3, t);
        set_sprite_tile(TORCH_GAUGE_BODY_4, TORCH_GAUGE_ZERO);
      } else if (player.torch_gauge > 8) {
        const uint8_t t = TORCH_GAUGE_ZERO + (player.torch_gauge - 8);
        set_sprite_tile(TORCH_GAUGE_BODY_1, TORCH_GAUGE_ZERO + 8);
        set_sprite_tile(TORCH_GAUGE_BODY_2, t);
        set_sprite_tile(TORCH_GAUGE_BODY_3, TORCH_GAUGE_ZERO);
        set_sprite_tile(TORCH_GAUGE_BODY_4, TORCH_GAUGE_ZERO);
      } else {
        const uint8_t t = TORCH_GAUGE_ZERO + player.torch_gauge;
        set_sprite_tile(TORCH_GAUGE_BODY_1, t);
        set_sprite_tile(TORCH_GAUGE_BODY_2, TORCH_GAUGE_ZERO);
        set_sprite_tile(TORCH_GAUGE_BODY_3, TORCH_GAUGE_ZERO);
        set_sprite_tile(TORCH_GAUGE_BODY_4, TORCH_GAUGE_ZERO);
      }
    } else {
      set_sprite_tile(TORCH_GAUGE_FLAME, FLAME_TILE_1);
      set_sprite_tile(TORCH_GAUGE_BODY_1, TORCH_GAUGE_ZERO);
      set_sprite_tile(TORCH_GAUGE_BODY_2, TORCH_GAUGE_ZERO);
      set_sprite_tile(TORCH_GAUGE_BODY_3, TORCH_GAUGE_ZERO);
      set_sprite_tile(TORCH_GAUGE_BODY_4, TORCH_GAUGE_ZERO);
    }
  } else {
    set_sprite_tile(TORCH_GAUGE_FLAME, SPRITE_TILE_CLEAR);
    set_sprite_tile(TORCH_GAUGE_BODY_1, SPRITE_TILE_CLEAR);
    set_sprite_tile(TORCH_GAUGE_BODY_2, SPRITE_TILE_CLEAR);
    set_sprite_tile(TORCH_GAUGE_BODY_3, SPRITE_TILE_CLEAR);
    set_sprite_tile(TORCH_GAUGE_BODY_4, SPRITE_TILE_CLEAR);
  }

  if (player.got_magic_key) {
    move_sprite(MAGIC_KEY_SPRITE_1, MAGIC_KEYS_X, MAGIC_KEYS_Y);
    move_sprite(MAGIC_KEY_SPRITE_2, MAGIC_KEYS_X, MAGIC_KEYS_Y + 8);
    move_sprite(MAGIC_KEY_QTY, MAGIC_KEYS_X + 9, MAGIC_KEYS_Y + 1);
    set_sprite_tile(MAGIC_KEY_QTY, MAGIC_KEY_NUM_0 + player.magic_keys);
  } else {
    move_sprite(MAGIC_KEY_SPRITE_1, 0, 0);
    move_sprite(MAGIC_KEY_SPRITE_2, 0, 0);
    move_sprite(MAGIC_KEY_QTY, 0, 0);
  }
}

/**
 * Clears the player HUD sprites.
 */
static void clear_hud(void) {
  move_sprite(TORCH_GAUGE_FLAME, 0, 0);
  move_sprite(TORCH_GAUGE_BODY_1, 0, 0);
  move_sprite(TORCH_GAUGE_BODY_2, 0, 0);
  move_sprite(TORCH_GAUGE_BODY_3, 0, 0);
  move_sprite(TORCH_GAUGE_BODY_4, 0, 0);
  move_sprite(MAGIC_KEY_SPRITE_1, 0, 0);
  move_sprite(MAGIC_KEY_SPRITE_1, 0, 0);
  move_sprite(MAGIC_KEY_QTY, 0, 0);
}

void clear_map_sprites(void) {
  clear_hero();
  clear_flames();
  clear_hud();
  clear_npcs();
}

/**
 * Updates the player's torch / flame.
 */
static void update_torch(void) {
  if (player.torch_color == FLAME_NONE)
    return;

  if (!update_timer(map.torch_timer))
    return;

  reset_timer(map.torch_timer);

  player.torch_gauge--;
  if (player.torch_gauge == 0) {
    player.torch_color = FLAME_NONE;
    core.load_sprite_palette(torch_gauge_palettes, TORCH_GAUGE_PALETTE, 1);
  }
}

/**
 * Initiates a fade out to state transition.
 * @param to_state Map state to enter once the fade out completes.
 */
static void map_fade_out(MapState to_state) {
  toggle_sprites();
  map.fade_to_state = to_state;
  fade_out();
  map.state = MAP_STATE_FADE_OUT;
}

/**
 * Initiates a fade in to state transition.
 * @param to_state Map state to enter once the fade in completes.
 */
static void map_fade_in(MapState to_state) {
  map.fade_to_state = to_state;
  fade_in();
  map.state = MAP_STATE_FADE_IN;
}

/**
 * Loads a map data tile at the given position in the current map.
 *
 * Note: this routine can safely be used with coordinates outside the current
 * map's dimensions. A blank tile will be returned in this case.
 *
 * Optimization: This method should not be used during rendering as it's
 * fairly slow. If you need to render based on its results create a buffer
 * of map tiles and draw them progressively instead.
 *
 * @param tile Pointer to a map tile where the data should be loaded.
 * @param x X-position in the map.
 * @param y Y-position in the map.
 */
static void get_map_tile(MapTile *tile, int8_t x, int8_t y) NONBANKED {
  const int8_t w = (int8_t)map.active_map->width;
  const int8_t h = (int8_t)map.active_map->height;

  if (x < 0 || y < 0 || x >= w || y >= h) {
    tile->blank = true;
    return;
  }

  const uint16_t offset = (uint16_t)(2 * (x + y * w));
  const uint8_t *data = map.active_map->data + offset;

  // Fetch banked tile & attribute data
  uint8_t _prev_bank = _current_bank;
  SWITCH_ROM(map.active_map->bank);
  uint8_t t = *data++;
  uint8_t attr = *data;
  SWITCH_ROM(_prev_bank);

  uint8_t map_tile = map_tile_lookup[t & MAP_TILE_MASK];
  uint8_t map_attr = t >> 6;

  tile->chest = NULL;
  tile->door = NULL;
  tile->lever = NULL;
  tile->sign = NULL;
  tile->sconce = NULL;
  tile->npc = NULL;

  TileHashEntry *entry = get_hash_entry(x, y);
  if (entry) {
    switch(entry->type) {
    case HASH_TYPE_CHEST:
      Chest *chest = (Chest *)entry->data;
      tile->chest = chest;
      if (is_chest_open(chest->id))
        map_tile += 2;
      break;
    case HASH_TYPE_LEVER:
      Lever *lever = (Lever *)entry->data;
      tile->lever = lever;
      if (!is_lever_on(lever->id))
        break;
      if (map_tile == 0x8E)
        map_tile = 0xA0;
      else
        map_tile = 0x8C;
      break;
    case HASH_TYPE_DOOR:
      Door *door = (Door *)entry->data;
      tile->door = door;
      if (is_locked_door(door->id))
        map_attr = MAP_WALL;
      else {
        map_tile = door->type;
        map_attr = MAP_EXIT;
      }
      break;
    case HASH_TYPE_SIGN:
      Sign *sign = (Sign *)entry->data;
      tile->sign = sign;
      break;
    case HASH_TYPE_SCONCE:
      Sconce *sconce = (Sconce *)entry->data;
      tile->sconce = sconce;
      break;
    case HASH_TYPE_NPC:
      NPC *npc = (NPC *)entry->data;
      tile->npc = npc;
      if (is_npc_visible(npc->id))
        map_attr = MAP_WALL;
      break;
    default:
    }
  }

  tile->bg_priority = attr & 0b10000000;
  attr &= 0b01111111;

  tile->blank = false;
  tile->tile = map_tile;
  tile->attr = attr;
  tile->map_attr = map_attr;
}

/**
 * Fills the progressive load tile buffer with data depending on the direction
 * the player is currently moving.
 */
static void load_tile_buffer(void) {
  // Set horizontal and vertical deltas based on move direction
  int8_t dx, dy;
  switch (map.move_direction) {
  case UP:
    dx = 0;
    dy = -1;
    break;
  case DOWN:
    dx = 0;
    dy = 1;
    break;
  case LEFT:
    dx = -1;
    dy = 0;
    break;
  case RIGHT:
    dx = 1;
    dy = 0;
    break;
  default:
    return;
  }

  // Reset the buffer position
  MapTile *t = tile_buf;
  map.buffer_pos = 0;

  // Update world coordinates
  map.x += dx;
  map.y += dy;

  // Update VRAM origin
  map.vram_x += 2 * dx;
  if (map.vram_x < 0)
    map.vram_x += 32;
  else if (map.vram_x >= 32)
    map.vram_x -= 32;

  map.vram_y += 2 * dy;
  if (map.vram_y < 0)
    map.vram_y += 32;
  else if (map.vram_y >= 32)
    map.vram_y -= 32;

  // Load data into buffer
  if (dy != 0) {
    map.buffer_max = MAP_HORIZ_LOADS;
    map.vram_d_col = 2;
    map.vram_d_row = 0;

    const int8_t vram_offset = dy < 0 ? 0 : 0x14;
    map.vram_col = map.vram_x;
    map.vram_row = map.vram_y + vram_offset;
    if (map.vram_row >= 0x20)
      map.vram_row -= 0x20;

    const int8_t map_offset = dy < 0 ? 0 : 10;
    for (int8_t x = map.x - 1; x < map.x + MAP_HORIZ_LOADS; x++, t++)
      get_map_tile(t, x, map.y - 1 + map_offset);
  } else if (dx != 0) {
    map.buffer_max = MAP_VERT_LOADS;
    map.vram_d_col = 0;
    map.vram_d_row = 2;

    const int8_t vram_offset = dx < 0 ? 0 : 0x16;
    map.vram_col = map.vram_x + vram_offset;
    if (map.vram_col >= 0x20)
      map.vram_col -= 0x20;
    map.vram_row = map.vram_y;

    const int8_t map_offset = dx < 0 ? -1 : 10;
    for (int8_t y = map.y - 1; y < map.y + MAP_VERT_LOADS; y++, t++)
      get_map_tile(t, map.x + map_offset, y);
  }
}

/**
 * Draws a map tile at the given location in VRAM.
 * @param vram Position in VRAM where the tile should be drawn.
 * @param map_tile Map tile data to draw.
 */
static void draw_map_tile(uint8_t *vram, MapTile *map_tile) {
  const uint8_t tile = map_tile->blank ? 0 : map_tile->tile;
  const uint8_t attr = map_tile->blank ? 0 :  map_tile->attr;

  // Set tilemap attributes
  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram, attr);
  set_vram_byte(vram + 1, attr);
  set_vram_byte(vram + 0x20, attr);
  set_vram_byte(vram + 0x20 + 1, attr);

  // Set tile from data
  VBK_REG = VBK_TILES;
  set_vram_byte(vram, tile);
  set_vram_byte(vram + 1, tile + 1);
  set_vram_byte(vram + 32, tile + 16);
  set_vram_byte(vram + 32 + 1, tile + 16 + 1);
}

/**
 * Draws a full screen with a border of prebuffered tiles for the current map
 * position. This needs to be called when loading a map, transition in and out
 * of the map system, etc. as it sets initial positions and data required for
 * progressive loading of tiles based on movement.
 */
static void refresh_map_screen(void) {
  uint8_t *vram = VRAM_BACKGROUND;
  MapTile tile;

  const int8_t screen_tiles_x = 10;
  const int8_t screen_tiles_y = 9;

  for (int8_t y = map.y - 1; y < map.y + screen_tiles_y + 1; y++) {
    for (int8_t x = map.x - 1; x < map.x + screen_tiles_x + 1; x++) {
      get_map_tile(&tile, x, y);
      draw_map_tile(vram, &tile);
      vram += 2;
    }
    vram += 32 + (32 - MAP_HORIZ_LOADS * 2);
  }

  map.scroll_x = 16;
  map.scroll_y = 16;
  map.vram_x = 0;
  map.vram_y = 0;
  move_bkg(map.scroll_x, map.scroll_y);
}

/**
 * Determines a VRAM address for the upper-left graphic tile in a given
 * direction relative to where the player is on the screen.
 * @param d Direction relative to the hero. Use `HERE` for the tile beneath.
 * @return A pointer in bg VRAM for the tile relative to the hero's posiiton.
 */
static uint8_t *get_local_vram(Direction d) {
  int8_t col = (int8_t)((map.scroll_x + 16 * HERO_X_OFFSET) >> 3);
  int8_t row = (int8_t)((map.scroll_y + 16 * HERO_Y_OFFSET) >> 3);

  switch (d) {
  case UP:
    row -= 2;
    break;
  case DOWN:
    row += 2;
    break;
  case LEFT:
    col -= 2;
    break;
  case RIGHT:
    col += 2;
    break;
  }

  if (col < 0)
    col += 32;
  else if (col >= 32)
    col -= 32;

  if (row < 0)
    row += 32;
  else if (row >= 32)
    row -= 32;

  return VRAM_BACKGROUND_XY(col, row);
}

/**
 * Sets background priority on at the move destination.
 */
static void set_move_vram_bg_priority(uint8_t attr) {
  uint8_t *vram = get_local_vram(map.move_direction);
  const uint8_t a = attr | 0b10000000;

  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram, a);
  set_vram_byte(vram + 1, a);
  set_vram_byte(vram + 0x20, a);
  set_vram_byte(vram + 0x20 + 1, a);
}

/**
 * Clears the background priority in VRAM for the given tile.
 * @param tile Tile to clear.
 */
static void clear_vram_bg_priority(void) {
  MapTile *tile;
  uint8_t *vram;

  switch (map.move_direction) {
  case UP:
    vram = get_local_vram(DOWN);
    tile = local_tiles + DOWN;
    break;
  case DOWN:
    vram = get_local_vram(UP);
    tile = local_tiles + UP;
    break;
  case LEFT:
    vram = get_local_vram(RIGHT);
    tile = local_tiles + RIGHT;
    break;
  default:
  // case RIGHT:
    vram = get_local_vram(LEFT);
    tile = local_tiles + LEFT;
    break;
  }

  const uint8_t attr = tile->attr;
  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram, attr);
  set_vram_byte(vram + 1, attr);
  set_vram_byte(vram + 0x20, attr);
  set_vram_byte(vram + 0x20 + 1, attr);
}

/**
 * Updates the local tiles state based on the current map position.
 */
static void update_local_tiles(void) {
  uint8_t x = map.x + HERO_X_OFFSET;
  uint8_t y = map.y + HERO_Y_OFFSET;
  get_map_tile(local_tiles + HERE, x, y);
  get_map_tile(local_tiles + UP, x, y - 1);
  get_map_tile(local_tiles + DOWN, x, y + 1);
  get_map_tile(local_tiles + LEFT, x - 1, y);
  get_map_tile(local_tiles + RIGHT, x + 1, y);
}

/**
 * Resets all stateful objects on the flooor (chests, doors, npc, levers, etc.)
 */
static void reset_map_objects(void) {
  reset_object_hash();

  // Chests
  map.flags_chest_open = 0;
  map.flags_chest_locked = 0;

  const Chest *chest;
  for (chest = map.active_floor->chests; chest->id != END; chest++) {
    if (chest->locked)
      set_chest_locked(chest->id);
    hash_object(HASH_TYPE_CHEST, chest, chest->map_id, chest->col, chest->row);
  }

  // Levers
  map.flags_lever_on = 0;
  map.flags_lever_stuck = 0;

  const Lever *lever;
  for (lever = map.active_floor->levers; lever->id != END; lever++) {
    if (lever->stuck)
      stick_lever(lever->id);
    hash_object(HASH_TYPE_LEVER, lever, lever->map_id, lever->col, lever->row);
  }

  // Doors
  map.flags_door_locked = 0;

  const Door *door;
  for (door = map.active_floor->doors; door->id != END; door++) {
    set_door_locked(door->id);
    hash_object(HASH_TYPE_DOOR, door, door->map_id, door->col, door->row);
  }

  // Signs
  const Sign *sign;
  for (sign = map.active_floor->signs; sign->map_id != END; sign++)
    hash_object(HASH_TYPE_SIGN, sign, sign->map_id, sign->col, sign->row);

  // Sconces
  map.flags_sconce_lit = 0;
  const Sconce *sconce;
  for (sconce = map.active_floor->sconces; sconce->id != END; sconce++) {
    hash_object(HASH_TYPE_SCONCE, sconce,
      sconce->map_id, sconce->col, sconce->row);
    if (sconce->is_lit) {
      light_sconce(sconce->id, sconce->color);
    }
  }

  // NPCs
  init_npcs();
  map.npc_visible = 0;
  const NPC *npc;
  for (npc = map.active_floor->npcs; npc->id != END; npc++) {
    hash_object(HASH_TYPE_NPC, npc, npc->map_id, npc->col, npc->row);
    set_npc_visible(npc->id);
  }
}

/**
 * Initiates a progressive screen reload based on the active exit being taken
 * by the player.
 */
static void load_exit(void) {
  DISPLAY_OFF;
  const Exit *exit = map.active_exit;

  if (exit->to_floor) {
    map.execute_on_init = true;
    set_active_floor(exit->to_floor);
  }

  map.active_map = map.active_floor->maps + exit->to_map;
  map.hero_direction = exit->heading;

  set_hero_position(exit->to_col, exit->to_row);
  update_local_tiles();
  refresh_map_screen();
  clear_flames();
  clear_npcs();
  map_fade_in(MAP_STATE_EXIT_LOADED);
  DISPLAY_ON;
}

void take_exit(Exit *exit) {
  map.active_exit = exit;
  map_fade_out(MAP_STATE_LOAD_EXIT);
}

/**
 * Handles state updates when the player moves into an exit tile.
 * @return `true` if default move behavior should be prevented.
 */
static bool handle_exit(void) {
  uint8_t x = map.x + HERO_X_OFFSET;
  uint8_t y = map.y + HERO_Y_OFFSET;

  const Exit *exit;
  for (exit = map.active_floor->exits; exit->map_id != END; exit++) {
    if (exit->map_id == 0xFF)
      break;
    if (exit->map_id != map.active_map->id)
      continue;
    if (exit->col != x || exit->row != y)
      continue;

    sfx_stairs();
    map.active_exit = exit;
    map_fade_out(MAP_STATE_LOAD_EXIT);
    return true;
  }

  return false;
}

/**
 * Initiates player movement in the given direction if possible.
 * @param d Direction the player is to move.
 */
static void start_move(Direction d) {
  MapTile *destination = local_tiles + d;

  if (map.hero_direction != d) {
    map.hero_direction = d;
    play_wall_hit = true;
  }

  if (destination->map_attr == MAP_WALL) {
    if (play_wall_hit) {
      sfx_wall_hit();
      play_wall_hit = false;
    }
    return;
  } else {
    play_wall_hit = true;
  }

  map.move_direction = d;
  map.move_step = 0;
  map.state = MAP_STATE_MOVING;
  map.hero_state = HERO_WALKING;

  if (destination->bg_priority) {
    set_move_vram_bg_priority(destination->attr);
    map.bg_priority_set = true;
  }

  // Note: this updates map.x & map.y
  load_tile_buffer();
}

/**
 * Checks user input and initiates a map move if they're holding the d-pad in
 * a given direction.
 */
static void check_map_move(void) {
  if (is_down(J_UP))
    start_move(UP);
  else if (is_down(J_DOWN))
    start_move(DOWN);
  else if (is_down(J_LEFT))
    start_move(LEFT);
  else if (is_down(J_RIGHT))
    start_move(RIGHT);
}

/**
 * Draws the next tile in the buffer during a map movement.
 */
static void draw_next_buffer_tile(void) {
  if (map.buffer_pos >= map.buffer_max) {
    return;
  }

  MapTile *map_tile = tile_buf + map.buffer_pos;
  uint8_t *vram = VRAM_BACKGROUND_XY(map.vram_col, map.vram_row);

  const uint8_t tile = map_tile->blank ? 0 : map_tile->tile;
  const uint8_t attr = map_tile->blank ? 0 :  map_tile->attr;

  VBK_REG = VBK_ATTRIBUTES;
  set_vram_byte(vram, attr);
  set_vram_byte(vram + 1, attr);
  set_vram_byte(vram + 0x20, attr);
  set_vram_byte(vram + 0x20 + 1, attr);

  VBK_REG = VBK_TILES;
  set_vram_byte(vram, tile);
  set_vram_byte(vram + 1, tile + 1);
  set_vram_byte(vram + 0x20, tile + 16);
  set_vram_byte(vram + 0x20 + 1, tile + 16 + 1);

  map.buffer_pos++;

  map.vram_col += map.vram_d_col;
  if (map.vram_col >= 32)
    map.vram_col -= 32;
  map.vram_row += map.vram_d_row;
  if (map.vram_row >= 32)
    map.vram_row -= 32;
}

/**
 * Update handler for when the player is moving.
 */
static void update_map_move(void) {
  switch (map.move_direction) {
  case UP:
    map.scroll_y--;
    break;
  case DOWN:
    map.scroll_y++;
    break;
  case LEFT:
    map.scroll_x--;
    break;
  case RIGHT:
    map.scroll_x++;
    break;
  }

  draw_next_buffer_tile();

  move_bkg(map.scroll_x, map.scroll_y);

  if (++map.move_step < 16)
    return;

  update_local_tiles();

  if (map.bg_priority_set) {
    clear_vram_bg_priority();
    map.bg_priority_set = false;
  }

  map.state = MAP_STATE_WAITING;
  map.hero_state = HERO_STILL;

  if (on_move())
    return;

  MapTile *here = local_tiles + HERE;

  switch (here->map_attr) {
  case MAP_EXIT:
    if (on_exit())
      return;
    if (handle_exit())
      return;
    break;
  case MAP_SPECIAL:
    if (on_special())
      return;
    break;
  default:
  }

  check_map_move();
}

/**
 * Checks signs at the current location and opens the message if one is found.
 * @return `true` if a sign was found and is being opened.
 */
static bool check_signs(void) {
  int8_t x = hero_x();
  int8_t y = hero_y();

  const MapTile *tile = local_tiles + map.hero_direction;
  const Sign *sign = tile->sign;

  if (!sign)
    return false;

  map_textbox(sign->message);
  return true;
}

/**
 * Swaps a tile to its "ON" state based on it's root tileset. This is used to
 * toggle chests open, switch lever graphics, etc.
 * @param tile Map tile to swap.
 * @param vram Place in bg VRAM to perform the swap.
 */
static void tile_to_state_on(const MapTile *tile, uint8_t *vram) {
  const uint8_t t = tile->tile + 2;
  set_vram_byte(vram, t);
  set_vram_byte(vram + 1, t + 1);
  set_vram_byte(vram + 0x20, t + 0x10);
  set_vram_byte(vram + 0x20 + 1, t + 0x10 + 1);
}

/**
 * Swaps a tile's graphics to the given base tile.
 * @param vram VRAM address of the tile to swap.
 * @param t Base tile for the graphics to set.
 */
static void swap_tile_graphics(uint8_t *vram, uint8_t t) {
  set_vram_byte(vram, t);
  set_vram_byte(vram + 1, t + 1);
  set_vram_byte(vram + 0x20, t + 0x10);
  set_vram_byte(vram + 0x20 + 1, t + 0x10 + 1);
}

/**
 * Swaps a tile to its "OFF" state based on it's root tileset.
 * @param tile Map tile to swap.
 * @param vram Place in bg VRAM to perform the swap.
 */
static void tile_to_state_off(const MapTile *tile, uint8_t *vram) {
  const uint8_t t = tile->tile;
  set_vram_byte(vram, t);
  set_vram_byte(vram + 1, t + 1);
  set_vram_byte(vram + 0x20, t + 0x10);
  set_vram_byte(vram + 0x20 + 1, t + 0x10 + 1);
}

/**
 * Sets a chest as open and updates the graphics.
 * @param tile Map tile containing the chest.
 */
static void open_chest(const MapTile *tile) {
  const Chest *chest = tile->chest;
  if (!chest)
    return;
  set_chest_open(chest->id);
  tile_to_state_on(tile, get_local_vram(map.hero_direction));
}

/**
 * Checks for chests and handles chest interactions at the current location.
 * @return `true` to prevent the default behavior of the action handler.
 */
static bool check_chests(void) {
  const MapTile *tile = local_tiles + map.hero_direction;
  const Chest *chest = tile->chest;

  if (!chest)
    return false;

  if (map.flags_chest_open & chest->id)
    return false;


  if (chest->on_open) {
    if (chest->on_open(chest)) {
      open_chest(tile);
    }
    return true;
  }

  const bool locked = map.flags_chest_locked & chest->id;
  const bool has_keys = player.magic_keys > 0;
  bool used_key = false;

  if (locked && chest->magic_key_unlock) {
    if (player.magic_keys == 0) {
      map_textbox(str_maps_chest_key_locked);
      return true;
    }
    player.magic_keys--;
    used_key = true;
    map.flags_chest_locked &= ~chest->id;
  } else if (locked) {
    map_textbox(str_maps_chest_locked);
    return true;
  }

  open_chest(tile);

  if (chest->items) {
    for (const Item *item = chest->items; item->id != END; item++)
      add_items(item->id, item->quantity);
  }

  const char *message = (chest->open_msg) ?
    chest->open_msg :
    str_maps_chest_open;

  if (used_key) {
    char buf[96];
    sprintf(buf, "%s\f%s", str_maps_chest_unlock_key, message);
    map_textbox(buf);
  } else {
    map_textbox(message);
  }

  return true;
}

/**
 * Toggles a lever on and off and updates the graphics.
 * @param tile Map tile containing the lever.
 */
static void toggle_lever(const MapTile *tile) {
  const Lever *lever = tile->lever;
  if (!lever)
    return;

  toggle_lever_state(lever->id);

  uint8_t t = 0x8A;

  if (tile->tile == 0x8A || tile->tile == 0x8C) {
    // Non-wall lever
    t = is_lever_on(lever->id) ? 0x8C : 0x8A;
  } else {
    // Wall lever
    t = is_lever_on(lever->id) ? 0xA0 : 0x8E;
  }

  uint8_t *vram = get_local_vram(map.hero_direction);
  set_vram_byte(vram, t);
  set_vram_byte(vram + 1, t + 1);
  set_vram_byte(vram + 0x20, t + 0x10);
  set_vram_byte(vram + 0x20 + 1, t + 0x10 + 1);
}

/**
 * Opens the door at the given map tile.
 * @param tile Map tile containing the door.
 */
static void open_door(const MapTile *tile) {
  const Door *door = tile->door;
  if (!door)
    return;
  set_door_open(door->id);

  uint8_t *vram = get_local_vram(map.hero_direction);
  uint8_t tile_base = door->type;
  swap_tile_graphics(vram, tile_base);

  update_local_tiles();
}

/**
 * @return Vram at the given tile.
 * @param tx Tile x coordinate.
 * @param ty Tile y coordinate.
 */
static uint8_t *get_vram_at(int8_t tx, int8_t ty) {
  int8_t col = (int8_t)((map.scroll_x) >> 3);
  int8_t row = (int8_t)((map.scroll_y) >> 3);

  const int8_t dx = tx - map.x;
  col += dx * 2;
  const int8_t dy = ty - map.y;
  row += dy * 2;

  if (col < 0)
    col += 32;
  if (col >= 32)
    col -= 32;

  if (row < 0)
    row += 32;
  if (row >= 32)
    row -= 32;

  return VRAM_BACKGROUND_XY(col, row);
}

void open_door_by_id(DoorId id) {
  const Door *door;
  for (door = map.active_floor->doors; door->id != END; door++) {
    if (door->id == id)
      break;
  }

  if (door->id == END)
    return;

  set_door_open(id);

  if (
    map.active_map->id != door->map_id ||
    door->col < map.x - 1 ||
    door->col >= map.x + MAP_HORIZ_LOADS ||
    door->row < map.y - 1 ||
    door->row >= map.y + MAP_VERT_LOADS
  ) return;

  uint8_t *vram = get_vram_at(door->col, door->row);
  uint8_t tile_base = door->type;
  swap_tile_graphics(vram, tile_base);
}

/**
 * Checks for a lever in front of the player and handles its logic.
 */
static bool check_levers(void) {
  const MapTile *tile = local_tiles + map.hero_direction;
  const Lever *lever = tile->lever;

  if (!lever)
    return false;

  if (lever->one_shot && is_lever_on(lever->id)) {
    map_textbox(str_maps_lever_one_way);
    return true;
  }

  if (is_lever_stuck(lever->id)) {
    map_textbox(str_maps_lever_stuck);
    return true;
  }

  toggle_lever(tile);

  if (lever->on_pull)
    lever->on_pull(lever);

  return true;
}

/**
 * Checks to see if the player is interacting with a door.
 */
static bool check_doors(void) {
  const MapTile *tile = local_tiles + map.hero_direction;
  const Door *door = tile->door;

  if (!door)
    return false;

  if (!is_locked_door(door->id))
    return false;

  if (door->magic_key_unlock) {
    if (player.magic_keys > 0) {
      player.magic_keys--;
      map_textbox(str_maps_door_unlock_key);
      set_door_open(door->id);
      open_door(tile);
    } else {
      map_textbox(str_maps_door_locked_key);
    }
  } else {
    map_textbox(str_maps_door_locked);
  }

  return true;

}

/**
 * Lights the player's torch with the given flame color.
 * @param color Color of the flame.
 */
static void light_torch(FlameColor color) {
  player.torch_gauge = 32;
  player.torch_color = color;
  const palette_color_t *palette = torch_gauge_palettes + color * 4;
  core.load_sprite_palette(palette, TORCH_GAUGE_PALETTE, 1);
}

void light_sconce(SconceId id, FlameColor color) {
  map.flags_sconce_lit |= id;
  sconce_colors[get_sconce_index(id)] = color;
  set_sprite_prop(get_sconce_flame_sprite(id), FLAME_SPRITE_PROP | color);

  // Call the "on_lit" handler
  const Sconce *sconce;
  for (sconce = map.active_floor->sconces; sconce->id != END; sconce++) {
    if (sconce->id != id)
      continue;
    if (sconce->on_lit)
      sconce->on_lit(sconce);
    break;
  }
}

/**
 * Checks to see if the player is interacting with a sconce.
 */
static bool check_sconces(void) {
  const MapTile *tile = local_tiles + map.hero_direction;
  const Sconce *sconce = tile->sconce;

  if (!sconce)
    return false;

  if (is_sconce_lit(sconce->id)) {
    if (player.has_torch) {
      if (sconce->id == SCONCE_STATIC) {
        light_torch(sconce->color);
      } else {
        const uint8_t sconce_idx = get_sconce_index(sconce->id);
        light_torch(sconce_colors[sconce_idx]);
      }
    } else {
      map_textbox(str_maps_sconce_lit_no_torch);
    }
  } else {
    if (player.has_torch) {
      if (player.torch_gauge > 0) {
        light_sconce(sconce->id, player.torch_color);
      } else {
        map_textbox(str_maps_sconce_torch_not_lit);
      }
    } else {
      map_textbox(str_maps_sconce_no_torch);
    }
  }

  return true;
}

/**
 * Checks to see if the player is interacting with an NPC.
 */
static bool check_npcs(void) {
  const MapTile *tile = local_tiles + map.hero_direction;
  const NPC *npc = tile->npc;

  if (!npc)
    return false;

  if (!is_npc_visible(npc->id))
    return false;

  if (!npc->on_action)
    return false;

  return npc->on_action(npc);
}

/**
 * Checks to see if the player is attempting to perform an action by pressing
 * the A button.
 * @return `true` if an action was attempted.
 */
static bool check_action(void) {
  if (was_pressed(J_A)) {
    if (on_action())
      return true;
    if (check_signs())
      return true;
    if (check_chests())
      return true;
    if (check_doors())
      return true;
    if (check_sconces())
      return true;
    if (check_npcs())
      return true;
    return check_levers();
  }
  return false;
}

void set_active_floor(Floor *floor) BANKED {
  map.active_floor = floor;
  map.active_map = &floor->maps[floor->default_map];
  set_hero_position(floor->default_x, floor->default_y);
  core.load_bg_palette(map.active_floor->palettes, 0, 7);
  reset_map_objects();
}

/**
 * Initializes the world map system.
 */
static void initialize_world_map(void) {
  DISPLAY_OFF;

  core.load_font();
  core.load_object_tiles();
  core.load_dungeon_tiles();
  core.load_bg_palette(map.active_floor->palettes, 0, 7);

  text_writer.auto_page = AUTO_PAGE_OFF;
  textbox.init();

  init_map_menu();
  init_hero();
  init_npcs();
  init_flames();
  init_hud();
  update_local_tiles();
  refresh_map_screen();

  game_state = GAME_STATE_WORLD_MAP;

  DISPLAY_ON;
}

void init_world_map(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  initialize_world_map();
  map.execute_on_init = true;
  map.state = MAP_STATE_WAITING;
}

void start_battle(void) {
  map_fade_out(MAP_STATE_START_BATTLE);
}

void return_from_battle(void) NONBANKED {
  SWITCH_ROM(MAP_SYSTEM_BANK);
  initialize_world_map();
  map_fade_in(MAP_STATE_WAITING);
}

void update_map(void) {
  switch (map.state) {
  case MAP_STATE_INACTIVE:
    return;
  case MAP_STATE_WAITING:
    if (on_init())
      break;
    if (was_pressed(J_START)) {
      show_map_menu();
      return;
    }
    if (!check_action())
      check_map_move();
    break;
  case MAP_STATE_MOVING:
    update_map_move();
    break;
  case MAP_STATE_TEXTBOX:
    textbox.update();
    if (textbox.state == TEXT_BOX_CLOSING && map.after_textbox) {
      bool (*callback)(void) = map.after_textbox;
      map.after_textbox = NULL;
      if (callback())
        break;
    }
    if (textbox.state == TEXT_BOX_CLOSED)
      map.state = MAP_STATE_WAITING;
    break;
  case MAP_STATE_FADE_OUT:
    if (fade_update())
      map.state = map.fade_to_state;
    break;
  case MAP_STATE_FADE_IN:
    if (fade_update()) {
      toggle_sprites();
      map.state = map.fade_to_state;
    }
    break;
  case MAP_STATE_LOAD_EXIT:
    load_exit();
    break;
  case MAP_STATE_EXIT_LOADED:
    start_move(map.active_exit->heading);
    break;
  case MAP_STATE_MENU:
    update_map_menu();
    if (map_menu.state == MAP_MENU_CLOSED) {
      map.state = MAP_STATE_WAITING;
      init_hero();
      init_hud();
      init_flames();
      init_npcs();
      break;
    }
    return;
  }

  update_torch();
  update_hero();
  update_flames();
  update_hud();
  update_npcs();
}

void update_world_map(void) NONBANKED {
  if (map.state == MAP_STATE_START_BATTLE) {
    map.state = MAP_STATE_INACTIVE;
    DISPLAY_OFF;
    init_battle();
  } else {
    update_map();
  }
}

void draw_world_map(void) {
}
