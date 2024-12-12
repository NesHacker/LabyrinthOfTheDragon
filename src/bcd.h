#ifndef _BCD_H
#define _BCD_H

#include <stdint.h>

typedef union bcd_t {
  struct {
    uint8_t d0;
    uint8_t d1;
    uint8_t d2;
    uint8_t d3;
  };
  uint32_t data;
} bcd_t;


// TODO Document me
void to_bcd8(uint8_t value, bcd_t *result);
void to_bcd16(uint16_t value, bcd_t *result);

#endif