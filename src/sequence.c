#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>

void next(uint32_t MASK, uint32_t *STATE_LFSR) {
  uint32_t BIT = *STATE_LFSR & 1; // BIT ← lsbit(STATE_LFSR) 
  STATE_LFSR = *STATE_LFSR >> 1; // STATE_LSFR ← STATE_LFSR >> 1 
  if (BIT == 1) STATE_LFSR = *STATE_LFSR ^ MASK; // if (BIT = 1)  STATE_LFSR ← STATE_LFSR xor MASK 
  uint32_t STEP = *STATE_LFSR & 0b11;  // STEP ← STATE_LFSR and 0b11 
  return STEP;
}
