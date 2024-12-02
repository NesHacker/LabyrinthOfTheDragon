#pragma bank 2

#include <gb/gb.h>
#include <gbdk/incbin.h>

// Example Area Maps (2 x 512 bytes) = 1K
INCBIN(map_example_0, "res/maps/map_example_0.bin")
INCBIN(map_example_1, "res/maps/map_example_1.bin")

// 15K of 16K remaining
