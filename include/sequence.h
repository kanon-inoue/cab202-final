#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

void next(uint32_t MASK, uint32_t *STATE_LFSR);