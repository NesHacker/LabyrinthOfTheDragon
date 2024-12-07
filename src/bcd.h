#ifndef _BCD_H
#define _BCD_H

#include <stdint.h>

typedef union bcd_t {
  struct {
    uint8_t ones;
    uint8_t tens;
  };
  uint16_t data;
} bcd_t;

void to_bcd(uint8_t value, bcd_t *result);

#endif