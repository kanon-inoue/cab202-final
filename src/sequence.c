#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>


// B. The Sequence
uint32_t MASK = 0xE2023CAB;
uint32_t STATE_LFSR = 0x11186267;

void next(uint32_t MASK, uint32_t *STATE_LFSR) {
  uint32_t BIT = *STATE_LFSR & 1; // BIT ← lsbit(STATE_LFSR) 
  STATE_LFSR = *STATE_LFSR >> 1; // STATE_LSFR ← STATE_LFSR >> 1 
  if (BIT == 1) STATE_LFSR = *STATE_LFSR ^ MASK; // if (BIT = 1)  STATE_LFSR ← STATE_LFSR xor MASK 
  uint32_t STEP = *STATE_LFSR & 0b11;  // STEP ← STATE_LFSR and 0b11 
  return STEP;
}



// A STEP value of 00 means that the tone E(high) will be played, 
// and the user must press pushbutton S1 to reproduce that step, a STEP value of 01 means C♯ is played, and so forth.