#ifndef _FLAGS_H
#define _FLAGS_H

#include <stdint.h>
#include <stdbool.h>


/**
 * @param b Bit for the flag mask.
 * @return mask for a flag at the given bit.
 */
#define FLAG(b) (1 << (b))

/**
 * Enumeration of names for each page in the global game flags.
 */
typedef enum FlagPage {
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

#endif
