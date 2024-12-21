#pragma bank 4

#include <stdio.h>

#include "encounter.h"
#include "item.h"
#include "strings.h"


/**
 * Sets the buff for an item. Uses the first available status effect slot or
 * the last slot if none are available. HAs no effect if the buff is already
 * active.
 * @param s Status effect to apply.
 * @param flag Flag for the status effect.
 * @param text Text to display upon successful application of the item buff.
 */
void set_item_buff(StatusEffect s, uint8_t flag, const char *text) {
  if (encounter.item_effects & flag)
    return;

  StatusEffectInstance *slot = NULL;
  StatusEffectInstance *effect;

  // Try to find the first effect with the same buff to overwrite
  effect = encounter.player_status_effects;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS - 1; k++, effect++) {
    if (!effect->active)
      continue;
    if (effect->effect == s) {
      slot = effect;
      break;
    }
  }

  // If not try to find the first inactive slot
  if (!slot) {
    effect = encounter.player_status_effects;
    for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS - 1; k++, effect++) {
      if (!effect->active) {
        slot = effect;
        break;
      }
    }
  }

  // If that fails, find the first active non-perpetual effect to overwrite
  if (!slot) {
    effect = encounter.player_status_effects;
    for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS - 1; k++, effect++) {
      if (!effect->active)
        continue;
      if (effect->duration == EFFECT_DURATION_PERPETUAL)
        continue;
      slot = effect;
      break;
    }
  }

  // You're shit out of luck and just wasted an item
  if (!slot) {
    sprintf(battle_pre_message, str_items_use_failed);
    skip_post_message = true;
    return;
  }

  sprintf(battle_pre_message, text);
  skip_post_message = true;

  slot->effect = s;
  slot->flag = flag;
  slot->active = true;
  slot->tier = A_TIER;
  slot->duration = EFFECT_DURATION_PERPETUAL;
  encounter.item_effects |= flag;
}

/**
 * Performs healing with an item. Sets the battle messages accordingly.
 * @param current Current value for the stat to heal (hp or sp).
 * @param max Max value for the stat.
 * @param factor Healing factor of the item.
 * @param format Format string to use for the results message.
 */
inline uint16_t item_heal(
  uint16_t current,
  uint16_t max,
  uint16_t factor,
  const char *format
) {
  uint16_t h = factor * max;
  h >>= 4;
  uint16_t result = current + h;

  sprintf(battle_pre_message, format, h);
  skip_post_message = true;

  return result <= max ? result : max;
}

/**
 * Potion battle action. Do not call directly.
 * @see use_item
 */
inline void use_potion(void) {
  const char *format = str_items_use_potion;
  player.hp = item_heal(player.hp, player.max_hp, POTION_HEAL_FACTOR, format);
}

/**
 * Ether battle action. Do not call directly.
 * @see use_item
 */
inline void use_ether(void) {
  const char *format = is_magic_class() ?
    str_items_use_ether_mp :
    str_items_use_ether_sp;
  player.sp = item_heal(player.sp, player.max_sp, ETHER_HEAL_FACTOR, format);
}

inline void use_remedy(void) {
  StatusEffectInstance *effect = encounter.player_status_effects;
  for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
    if (!effect->active)
      continue;
    if (is_debuff(effect->effect))
      effect->active = false;
  }
  sprintf(battle_pre_message, str_items_use_remedy);
  skip_post_message = true;
}

inline void use_atkup_potion(void) {
  set_item_buff(BUFF_ATK_UP, FLAG_BUFF_ATK_UP, str_items_use_atkup);
}

inline void use_defup_potion(void) {
  set_item_buff(BUFF_DEF_UP, FLAG_BUFF_DEF_UP, str_items_use_defup);
}

inline void use_elixer(void) {
  player.hp = player.max_hp;
  player.sp = player.max_sp;
  sprintf(battle_pre_message, str_items_use_elixer);
  skip_post_message = true;
}

inline void use_regen_potion(void) {
  set_item_buff(BUFF_REGEN, FLAG_BUFF_REGEN, str_items_use_regen);
}

inline void use_haste_potion(void) {
  set_item_buff(BUFF_HASTE, FLAG_BUFF_HASTE, str_items_use_haste);
}

bool can_use_item(ItemId id) BANKED {
  Item *i = inventory + id;
  if (i->quantity == 0)
    return false;

  switch (i->id) {
  case ITEM_POTION:
    return player.hp < player.max_hp;
  case ITEM_ETHER:
    return player.sp < player.max_sp;
  case ITEM_REMEDY:
    StatusEffectInstance *effect = encounter.player_status_effects;
    for (uint8_t k = 0; k < MAX_ACTIVE_EFFECTS; k++, effect++) {
      if (effect->active && is_debuff(effect->effect))
        return true;
    }
    return false;
  case ITEM_ELIXER:
    return player.hp < player.max_hp || player.sp < player.max_sp;
  case ITEM_ATK_UP:
    return !(encounter.item_effects & FLAG_BUFF_ATK_UP);
  case ITEM_DEF_UP:
    return !(encounter.item_effects & FLAG_BUFF_DEF_UP);
  case ITEM_REGEN:
    return !(encounter.item_effects & FLAG_BUFF_REGEN);
  case ITEM_HASTE:
    return !(encounter.item_effects & FLAG_BUFF_HASTE);
  default:
    return false;
  }
}

void use_item(ItemId id) BANKED {
  Item *i = inventory + id;
  switch (i->id) {
    case ITEM_POTION:
      use_potion();
      return;
    case ITEM_ETHER:
      use_ether();
      return;
    case ITEM_REMEDY:
      use_remedy();
      return;
    case ITEM_ATK_UP:
      use_atkup_potion();
      return;
    case ITEM_DEF_UP:
      use_defup_potion();
      return;
    case ITEM_ELIXER:
      use_elixer();
      return;
    case ITEM_REGEN:
      use_regen_potion();
      return;
    case ITEM_HASTE:
      use_haste_potion();
      return;
    default:
  }
}

Item inventory[8] = {
  { ITEM_POTION, str_misc_potion, 0 },
  { ITEM_ETHER, str_misc_ether, 0 },
  { ITEM_REMEDY, str_misc_remedy, 0 },
  { ITEM_ATK_UP, str_misc_atk_up_potion, 0 },
  { ITEM_DEF_UP, str_misc_def_up_potion, 0 },
  { ITEM_ELIXER, str_misc_elixer, 0 },
  { ITEM_REGEN, str_misc_regen_pot, 0 },
  { ITEM_HASTE, str_misc_haste_pot, 0 },
};
