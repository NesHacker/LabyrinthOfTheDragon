#include "flags.h" 

uint8_t flags[32] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

inline bool check_flags(FlagPage page, uint8_t mask) {
  return flags[page] & mask;
}

inline void set_flags(FlagPage page, uint8_t mask) {
  flags[page] |= mask;
}

inline void unset_flags(FlagPage page, uint8_t mask) {
  flags[page] &= ~mask;
}

inline void toggle_flags(FlagPage page, uint8_t mask) {
  flags[page] ^= mask;
}
